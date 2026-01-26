#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include "ntg.h"
#include "nt.h"
#include "core/loop/_ntg_loop.h"

static void init_default(ntg_loop* loop);
// function assumes that pending_stage_flag = true
static void update_stage(ntg_loop* loop);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_loop* ntg_loop_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_LOOP,
        .deinit_fn = (ntg_entity_deinit_fn)ntg_loop_deinit,
        .system = system
    };

    ntg_loop* new = (ntg_loop*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_loop_init(ntg_loop* loop, ntg_stage* init_stage, ntg_renderer* renderer,
                   unsigned int framerate, unsigned int workers)
{
    assert(loop != NULL);
    assert(init_stage != NULL);
    assert(!init_stage->_loop);
    assert(renderer != NULL);

    init_default(loop);

    loop->_framerate = (framerate <= NTG_LOOP_FRAMERATE_MAX) ?
        framerate : NTG_LOOP_FRAMERATE_MAX;
    loop->_arena = sarena_create(1000000);
    assert(loop->_arena);

    loop->_renderer = renderer;

    loop->_task_runner = malloc(sizeof(ntg_task_runner));
    assert(loop->_task_runner);
    loop->_platform = malloc(sizeof(ntg_platform));
    assert(loop->_platform);

    workers = (workers <= NTG_LOOP_WORKERS_MAX) ? workers : NTG_LOOP_WORKERS_MAX;
    _ntg_platform_init(loop->_platform, loop);
    _ntg_task_runner_init(loop->_task_runner, loop->_platform, workers, loop);
    
    ntg_loop_set_stage(loop, init_stage);
}

void ntg_loop_deinit(ntg_loop* loop)
{
    assert(loop != NULL);
    assert(!loop->_running);
    if(loop->_running) return;

    sarena_destroy(loop->_arena);

    if(!loop->__force_break)
    {
        _ntg_task_runner_deinit(loop->_task_runner);
        _ntg_platform_deinit(loop->_platform);

        free(loop->_platform);
        free(loop->_task_runner);
    }

    ntg_loop_set_stage(loop, NULL);

    init_default(loop);
}

bool ntg_loop_dispatch_event(ntg_loop* loop, struct nt_event event)
{
    if(loop->_stage)
    {
        ntg_stage* stage = loop->_stage;
        if(event.type == NT_EVENT_KEY)
        {
            struct nt_key_event key = *(struct nt_key_event*)event.data;

            if(stage->on_key_fn)
                return stage->on_key_fn(stage, key);
            else
                return false;
        }
        else if(event.type == NT_EVENT_MOUSE)
        {
            struct nt_mouse_event mouse = *(struct nt_mouse_event*)event.data;

            if(stage->on_mouse_fn)
                return stage->on_mouse_fn(stage, mouse);
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

ntg_loop_end_mode ntg_loop_run(ntg_loop* loop)
{
    assert(loop != NULL);
    assert(!loop->_running);

    size_t resize_counter = 0, sigwinch_counter = 0;

    /* loop */
    unsigned int timeout = 1000 / loop->_framerate;
    struct timespec ts_start, ts_end;
    int64_t process_elapsed_ns;
    uint64_t process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event event;
    struct nt_resize_event resize_event;
    unsigned int event_elapsed;
    struct ntg_event_loop_generic_data loop_event_data;

    nt_status _status;

    loop->_running = true;
    loop->__loop = true;
    loop->__force_break = false;
    nt_get_term_size(&loop->_app_size.x, &loop->_app_size.y);

    while(true)
    {
        if(!(loop->__loop)) break;
        if(loop->__pending_stage_flag)
        {
            update_stage(loop);
        }

        event_elapsed = nt_event_wait(&event, timeout, &_status);
        assert(_status == NT_SUCCESS);
        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        if(event.type == NT_EVENT_RESIZE)
        {
            resize_event = *(struct nt_resize_event*)event.data;
            loop->_app_size.x = resize_event.new_x;
            loop->_app_size.y = resize_event.new_y;

            resize_counter++;
        }
        if(event.type == NT_EVENT_SIGNAL)
        {
            sigwinch_counter++;
        }

        loop->on_event_fn(loop, event);

        // Frame end
        if(event.type == NT_EVENT_TIMEOUT)
        {
            _ntg_platform_execute_all(loop->_platform);

            timeout = 1000 / loop->_framerate;

            if(loop->_stage != NULL)
            {
                ntg_stage_compose(loop->_stage, loop->_app_size, loop->_arena);
                drawing = &loop->_stage->_drawing;
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

        // TODO
        loop_event_data.event = event;
        ntg_entity_raise_event_((ntg_entity*)loop, NTG_EVENT_LOOP_GENERIC,
                               &loop_event_data);

        clock_gettime(CLOCK_MONOTONIC, &ts_end);

        // Subtract time taken inside the loop iteration
        process_elapsed_ns = (int64_t)(ts_end.tv_sec - ts_start.tv_sec) * 1000000000LL
            + (int64_t)(ts_end.tv_nsec - ts_start.tv_nsec);
        // process_elapsed_ns = (process_elapsed_ns > 0) ? process_elapsed_ns : 0;

        process_elapsed_ms = process_elapsed_ns / 1000000LL;
        loop->_elapsed += (event_elapsed + process_elapsed_ms);
        timeout = (timeout > process_elapsed_ms) ? timeout - process_elapsed_ms : 0;
    }

    enum ntg_loop_end_mode status;
    if(!loop->__force_break)
    {
        status = NTG_LOOP_END_CLEAN;
    }
    else
    {
        status = NTG_LOOP_END_FORCE;
        _ntg_platform_invalidate(loop->_platform);
        _ntg_task_runner_invalidate(loop->_task_runner);
    }

    ntg_log_log("%d %d", resize_counter, sigwinch_counter);

    return status;
}

bool ntg_loop_break(ntg_loop* loop, ntg_loop_end_mode end_mode)
{
    assert(loop != NULL);

    if(end_mode == NTG_LOOP_END_FORCE)
    {
        loop->__loop = false;
        loop->_running = false;
        loop->__force_break = true;

        return true;
    }
    else
    {
        if(!_ntg_task_runner_is_running(loop->_task_runner))
        {
            loop->__loop = false;
            loop->_running = false;

            return true;
        }

        return false;
    }
}

void ntg_loop_set_stage(ntg_loop* loop, ntg_stage* stage)
{
    assert(loop != NULL);

    if(loop->_running)
    {
        loop->__pending_stage_flag = true;
        loop->__pending_stage = stage;
    }
    else
    {
        if(loop->_stage)
        {
            _ntg_stage_set_loop(loop->_stage, NULL);
        }
        if(stage)
        {
            _ntg_stage_set_loop(stage, loop);
        }

        loop->_stage = stage;
    }
}

void ntg_task_runner_execute(ntg_task_runner* task_runner, 
        void (*task_fn)(void* data, ntg_platform* platform),
        void* data)
{
    struct ntg_task task = {
        .task_fn = task_fn,
        .data = data
    };
    _ntg_task_runner_execute(task_runner, task);
}

void ntg_platform_execute_later(ntg_platform* platform, 
                                void (*task_fn)(void* data),
                                void* data)
{
    struct ntg_ptask task = {
        .task_fn = task_fn,
        .data = data
    };
    _ntg_platform_execute_later(platform, task);
}


static void init_default(ntg_loop* loop)
{
    loop->on_event_fn = ntg_loop_dispatch_event;

    loop->_framerate = 0;
    loop->_running = false;
    loop->_app_size = ntg_xy(0, 0);
    loop->_stage = NULL;
    loop->_renderer = NULL;
    loop->_elapsed = 0;
    loop->_frame = 0;
    loop->_arena = NULL;

    loop->_platform = NULL;
    loop->_task_runner = NULL;

    loop->__loop = false;
    loop->__force_break = false;
    loop->__pending_stage = NULL;
    loop->__pending_stage_flag = false;

    loop->data = NULL;
}

static void update_stage(ntg_loop* loop)
{
    ntg_stage* old = loop->_stage;
    ntg_stage* new = loop->__pending_stage;

    if(old)
    {
        _ntg_stage_set_loop(old, NULL);
    }
    if(new)
    {
        assert(!new->_loop);

        _ntg_stage_set_loop(new, loop);
    }

    loop->_stage = new;
    loop->__pending_stage = NULL;
    loop->__pending_stage_flag = false;
}
