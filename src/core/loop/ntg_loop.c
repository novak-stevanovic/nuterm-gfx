#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include "ntg.h"
#include "nt.h"
#include "core/loop/ntg_loop_internal.h"
#include "shared/ntg_shared_internal.h"

static void init_default(ntg_loop* loop);
static void update_stage(ntg_loop* loop);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

bool ntg_loop_dispatch_event(ntg_loop* loop, struct nt_event event)
{
    if(!loop) return false;

    if(loop->_stage)
    {
        ntg_stage* stage = loop->_stage;
        if(event.type == NT_EVENT_KEY)
        {
            struct nt_key_event key = *(struct nt_key_event*)event.data;
            return ntg_stage_feed_key(stage, key);
        }
        else if(event.type == NT_EVENT_MOUSE)
        {
            struct nt_mouse_event mouse = *(struct nt_mouse_event*)event.data;
            return ntg_stage_feed_mouse(stage, mouse);
        }
        else return false;
    }
    else return false;
}

void ntg_loop_init(ntg_loop* loop,
        ntg_stage* init_stage,
        ntg_renderer* renderer,
        unsigned int framerate,
        unsigned int workers,
        bool (*on_event_fn)(ntg_loop* loop, struct nt_event event),
        ntg_status* out_status)
{
    ntg_init_status(out_status);

    if(!loop || !init_stage)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(init_stage->_loop)
        ntg_vreturn(out_status, NTG_ERR_STAGE_HAS_LOOP);

    init_default(loop);

    loop->_framerate = (framerate <= NTG_LOOP_FRAMERATE_MAX) ?
        framerate : NTG_LOOP_FRAMERATE_MAX;

    loop->_renderer = renderer;

    loop->_task_runner = malloc(sizeof(ntg_task_runner));
    loop->_platform = malloc(sizeof(ntg_platform));

    if(!loop->_task_runner || !loop->_platform)
    {
        free(loop->_task_runner);
        free(loop->_platform);

        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }

    workers = (workers <= NTG_LOOP_WORKERS_MAX) ? workers : NTG_LOOP_WORKERS_MAX;

    _ntg_platform_init(loop->_platform, loop);
    ntg_status _status;
    _ntg_task_runner_init(loop->_task_runner, loop->_platform, workers, loop, &_status);
    
    if(_status != NTG_SUCCESS)
    {
        _ntg_platform_deinit(loop->_platform);

        free(loop->_task_runner);
        free(loop->_platform);

        ntg_vreturn(out_status, _status);
    }

    ntg_loop_set_stage(loop, init_stage);

    loop->__on_event_fn = on_event_fn;
}

void ntg_loop_deinit(ntg_loop* loop, ntg_status* out_status)
{
    ntg_init_status(out_status);

    if(!loop)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(loop->_status == NTG_LOOP_RUNNING)
        ntg_vreturn(out_status, NTG_ERR_LOOP_RUNNING);

    if(loop->__exit_status == NTG_LOOP_EXIT_CLEAN)
    {
        _ntg_task_runner_deinit(loop->_task_runner);
        _ntg_platform_deinit(loop->_platform);

        free(loop->_platform);
        free(loop->_task_runner);
    }

    ntg_loop_set_stage(loop, NULL);

    init_default(loop);
}

void ntg_loop_deinit_(void* _loop)
{
    if(!_loop) return;

    ntg_loop_deinit(_loop, NULL);
}

ntg_loop_exit_status ntg_loop_run(ntg_loop* loop, ntg_status* out_status)
{
    ntg_init_status(out_status);

    if(!loop)
        ntg_return(NTG_LOOP_EXIT_ERROR, out_status, NTG_ERR_INVALID_ARG);

    if(loop->_status == NTG_LOOP_RUNNING)
        ntg_return(NTG_LOOP_EXIT_ERROR, out_status, NTG_ERR_LOOP_RUNNING);

    size_t resize_counter = 0, sigwinch_counter = 0;

    loop->_arena = sarena_create(2000000);
    if(!loop->_arena)
        ntg_return(NTG_LOOP_EXIT_ERROR, out_status, NTG_ERR_ALLOC_FAIL);

    bool owns_renderer;
    if(!loop->_renderer)
    {
        loop->_renderer = malloc(sizeof(ntg_def_renderer));
        if(!loop->_renderer)
        {
            sarena_destroy(loop->_arena);
            loop->_arena = NULL;

            ntg_return(NTG_LOOP_EXIT_ERROR, out_status, NTG_ERR_ALLOC_FAIL);
        }

        ntg_def_renderer_init((ntg_def_renderer*)loop->_renderer);
        owns_renderer = true;
    }
    else
        owns_renderer = false;

    /* loop */
    unsigned int timeout = 1000 / loop->_framerate;
    struct timespec ts_start, ts_end;
    int64_t process_elapsed_ns;
    uint64_t process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event event;
    struct nt_resize_event resize_event;
    unsigned int event_elapsed;

    nt_status _status;

    loop->_status = NTG_LOOP_RUNNING;
    nt_get_term_size(&loop->_app_size.x, &loop->_app_size.y);

    if(loop->_stage)
        _ntg_stage_set_size(loop->_stage, loop->_app_size);
    while(true)
    {
        if(loop->_status == NTG_LOOP_STOPPING) break;

        update_stage(loop);

        event_elapsed = nt_event_wait(&event, timeout, &_status);
        if(_status != NT_SUCCESS) continue;

        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        if(event.type == NT_EVENT_RESIZE)
        {
            resize_event = *(struct nt_resize_event*)event.data;
            loop->_app_size.x = resize_event.new_x;
            loop->_app_size.y = resize_event.new_y;

            if(loop->_stage)
            {
                _ntg_stage_set_size(loop->_stage, loop->_app_size);
            }
            ntg_log_log("RESIZE %d %d", loop->_app_size.x, loop->_app_size.y);

            resize_counter++;
        }
        else if(event.type == NT_EVENT_SIGNAL)
        {
            sigwinch_counter++;
        }

        if(loop->__on_event_fn)
            loop->__on_event_fn(loop, event);

        // Frame end
        if(event.type == NT_EVENT_TIMEOUT)
        {
            _ntg_platform_execute_all(loop->_platform);

            timeout = 1000 / loop->_framerate;

            if(loop->_stage)
            {
                if(loop->_stage->_dirty)
                {
                    _ntg_stage_compose(loop->_stage, loop->_arena);
                    _ntg_stage_clean(loop->_stage);
                    // ntg_loop_break(loop, NTG_LOOP_STOP_CLEAN);
                }
                drawing = &(loop->_stage->_drawing);
            }
            else drawing = NULL;

            ntg_renderer_render(loop->_renderer, drawing, loop->_arena);

            sarena_rewind(loop->_arena);
            (loop->_frame)++;
        }
        else
        {
            event_elapsed = (timeout >= event_elapsed) ? event_elapsed : timeout;
            timeout -= event_elapsed;
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_end);

        // Subtract time taken inside the loop iteration
        process_elapsed_ns = (int64_t)(ts_end.tv_sec - ts_start.tv_sec) * 1000000000LL
            + (int64_t)(ts_end.tv_nsec - ts_start.tv_nsec);
        // process_elapsed_ns = (process_elapsed_ns > 0) ? process_elapsed_ns : 0;

        process_elapsed_ms = process_elapsed_ns / 1000000LL;
        loop->_elapsed += (event_elapsed + process_elapsed_ms);
        timeout = (timeout > process_elapsed_ms) ? timeout - process_elapsed_ms : 0;
    }

    loop->_status = NTG_LOOP_END;
    if(loop->__exit_status == NTG_LOOP_EXIT_PREMATURE)
    {
        _ntg_platform_invalidate(loop->_platform);
        _ntg_task_runner_invalidate(loop->_task_runner);
    }

    if(owns_renderer)
    {
        ntg_def_renderer_deinit((ntg_def_renderer*)loop->_renderer);
        free(loop->_renderer);
        loop->_renderer = NULL;
    }

    if(loop->_stage)
    {
        ntg_loop_set_stage(loop, NULL);
    }

    sarena_destroy(loop->_arena);
    loop->_arena = NULL;

    return loop->__exit_status;
}

bool ntg_loop_break(ntg_loop* loop, ntg_loop_stop_mode stop_mode)
{
    if(!loop) return true;

    if(stop_mode == NTG_LOOP_STOP_FORCE)
    {
        if(_ntg_task_runner_is_running(loop->_task_runner))
        {
            loop->_status = NTG_LOOP_STOPPING;
            loop->__exit_status = NTG_LOOP_EXIT_PREMATURE;
        }
        else
        {
            loop->_status = NTG_LOOP_STOPPING;
            loop->__exit_status = NTG_LOOP_EXIT_CLEAN;
        }

        return true;
    }
    else
    {
        if(!_ntg_task_runner_is_running(loop->_task_runner))
        {
            loop->_status = NTG_LOOP_STOPPING;
            loop->__exit_status = NTG_LOOP_EXIT_CLEAN;

            return true;
        }

        return false;
    }
}

void ntg_loop_set_stage(ntg_loop* loop, ntg_stage* stage)
{
    if(!loop) return;

    if((loop->_status == NTG_LOOP_RUNNING) || (loop->_status == NTG_LOOP_STOPPING))
    {
        loop->__pending_stage = stage;
    }
    else
    {
        ntg_stage* old_stage = loop->_stage;

        if(old_stage == stage) return;

        if(old_stage)
        {
            _ntg_stage_set_loop(old_stage, NULL);
            _ntg_stage_set_size(old_stage, ntg_xy(0, 0));
        }
        if(stage)
        {
            // If stage already has loop
            if(stage->_loop)
            {
                ntg_loop_set_stage(stage->_loop, NULL);
            }

            _ntg_stage_set_loop(stage, loop);
            _ntg_stage_set_size(stage, loop->_app_size);
            ntg_stage_mark_dirty(stage);
        }

        loop->_stage = stage;
        loop->__pending_stage = stage;

        if(old_stage && old_stage->hooks.on_loop_chng_fn)
            old_stage->hooks.on_loop_chng_fn(old_stage, loop, NULL);

        if(stage && stage->hooks.on_loop_chng_fn)
            stage->hooks.on_loop_chng_fn(stage, NULL, loop);
    }
}

void ntg_task_runner_execute(
        ntg_task_runner* task_runner, 
        void (*task_fn)(void* data, ntg_platform* platform),
        void* data)
{
    if(!task_runner || !task_fn) return;

    struct ntg_task task = {
        .task_fn = task_fn,
        .data = data
    };
    _ntg_task_runner_execute(task_runner, task);
}

void ntg_platform_execute_later(
        ntg_platform* platform, 
        void (*task_fn)(void* data),
        void* data)
{
    if(!platform || !task_fn) return;

    struct ntg_ptask task = {
        .task_fn = task_fn,
        .data = data
    };
    _ntg_platform_execute_later(platform, task);
}


static void init_default(ntg_loop* loop)
{
    if(!loop) return;

    loop->__on_event_fn = ntg_loop_dispatch_event;

    loop->_status = NTG_LOOP_READY;
    loop->__exit_status = NTG_LOOP_EXIT_CLEAN;
    loop->_framerate = 0;
    loop->_app_size = ntg_xy(0, 0);
    loop->_stage = NULL;
    loop->_renderer = NULL;
    loop->_elapsed = 0;
    loop->_frame = 0;
    loop->_arena = NULL;

    loop->_platform = NULL;
    loop->_task_runner = NULL;

    loop->__pending_stage = NULL;

    loop->data = NULL;
}

static void update_stage(ntg_loop* loop)
{
    if(!loop) return;

    ntg_stage* old = loop->_stage;
    ntg_stage* new = loop->__pending_stage;

    if(old == new) return;

    if(new && new->_loop) return;

    if(old)
    {
        _ntg_stage_set_loop(old, NULL);
        _ntg_stage_set_size(old, ntg_xy(0, 0));
    }
    if(new)
    {
        _ntg_stage_set_loop(new, loop);
        _ntg_stage_set_size(new, loop->_app_size);
        ntg_stage_mark_dirty(new);
    }

    loop->_stage = new;
    loop->__pending_stage = loop->_stage;
}
