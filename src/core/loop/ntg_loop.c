#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include "nt.h"
#include <stdlib.h>
#include <time.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static struct
{
    enum ntg_loop_status status;

    struct
    {
        ntg_renderer* renderer;
        bool _init_renderer, _owns_renderer;
        bool (*dispatch_event_fn)(struct nt_event event);
        sarena* arena;
    } init;

    struct
    {
        unsigned int framerate;
        ntg_stage* stage;
        struct ntg_xy app_size;
    } running;

} loop = {0};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

static void init_default()
{
    loop.status = NTG_LOOP_DEINIT;

    loop.init.renderer = NULL;
    loop.init._owns_renderer = false;
    loop.init._init_renderer = false;
    loop.init.dispatch_event_fn = NULL;
    loop.init.arena = NULL;

    loop.running.stage = NULL;
    loop.running.framerate = 0;
    loop.running.app_size = ntg_xy(0, 0);

    // loop.frame_count = 0;
}

static void deinit()
{
    if(loop.init.renderer && loop.init._owns_renderer)
    {
        if(loop.init._init_renderer)
            ntg_renderer_vdeinit(loop.init.renderer);

        free(loop.init.renderer);
    }

    if(loop.init.arena)
        sarena_destroy(loop.init.arena);

    init_default();
}

void ntg_loop_init(
        ntg_renderer* renderer,
        bool (*dispatch_event_fn)(struct nt_event event),
        unsigned int workers,
        size_t arena_size,
        ntg_stage* init_stage,
        int* out_status)
{
    ntg_set_out(out_status, 0);

    if(loop.status != NTG_LOOP_DEINIT)
        ntg_vreturn(out_status, NTG_ERR_LOOP_INVALID_STATE);

    /* Opts */

    if((arena_size == 0) || (workers == 0))
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    workers = _min2_uint(workers, NTG_LOOP_WORKERS_MAX);

    /* Arena */

    loop.init.arena = sarena_create(arena_size);
    if(!loop.init.arena)
    {
        deinit();
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
    }

    /* Renderer */

    if(renderer) // CUSTOM RENDERER
    {
        loop.init.renderer = renderer;
        loop.init._owns_renderer = false;
    }
    else
    {
        loop.init.renderer = malloc(sizeof(ntg_default_renderer));
        if(!loop.init.renderer)
        {
            deinit();
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL); 
        }

        int _status;
        ntg_default_renderer_init((ntg_default_renderer*)renderer, &_status);  

        switch(_status)
        {
            case 0: break;
            case NTG_ERR_ALLOC_FAIL:
                deinit();
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL); 
            default:
                deinit();
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED); 
        }

        loop.init._init_renderer = true;
        loop.init._owns_renderer = true;
    }

    loop.init.dispatch_event_fn = (dispatch_event_fn ?
            dispatch_event_fn :
            ntg_loop_dispatch_event_fn_default);
    loop.status = NTG_LOOP_READY;

    loop.running.stage = init_stage;
}

// What if there are active tasks?
void ntg_loop_deinit(int* out_status)
{
    if((loop.status == NTG_LOOP_RUNNING) || (loop.status == NTG_LOOP_STOPPING))
        ntg_vreturn(out_status, NTG_ERR_LOOP_INVALID_STATE);

    deinit();
}

ntg_loop_status ntg_loop_get_status()
{
    return loop.status;
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_loop_dispatch_event_fn_default(struct nt_event event)
{
    ntg_stage* stage = loop.running.stage;
    if(stage)
    {
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

/* ------------------------------------------------------ */
/* START/STOP */
/* ------------------------------------------------------ */

void ntg_loop_start(unsigned int framerate, int* out_status)
{
    if(loop.status != NTG_LOOP_READY)
        ntg_vreturn(out_status, NTG_ERR_LOOP_INVALID_STATE);

    framerate = _min2_uint(framerate, NTG_LOOP_FRAMERATE_MAX);
    loop.running.framerate = framerate;

    sarena_rewind(loop.init.arena);

    int _status;

    unsigned int timeout = 1000 / loop.running.framerate;
    struct timespec ts_start, ts_end;
    int64_t process_elapsed_ns;
    uint64_t process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event event;
    struct nt_resize_event resize_event;
    unsigned int event_elapsed;

    nt_get_term_size(&loop.running.app_size.x, &loop.running.app_size.y);

    int _recompose;

    loop.status = NTG_LOOP_RUNNING;
    if(loop.running.stage)
        _ntg_stage_set_size(loop.running.stage, loop.running.app_size);
    while(true)
    {
        if(loop.status == NTG_LOOP_STOPPING) break;

        update_stage(loop);

        event_elapsed = nt_event_wait(&event, timeout, &_status);
        if(_status != 0) continue;

        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        if(event.type == NT_EVENT_RESIZE)
        {
            resize_event = *(struct nt_resize_event*)event.data;
            loop.running.app_size.x = resize_event.new_x;
            loop.running.app_size.y = resize_event.new_y;

            if(loop.running.stage)
            {
                _ntg_stage_set_size(loop.running.stage, loop.running.app_size);
            }
            // resize_counter++;
        }
        else if(event.type == NT_EVENT_SIGNAL)
        {
            // sigwinch_counter++;
        }

        if(loop.init.dispatch_event_fn)
            loop.init.dispatch_event_fn(event);

        if(event.type == NT_EVENT_TIMEOUT)
        {
            // _ntg_platform_execute_all(loop.platform);

            timeout = 1000 / loop.running.framerate;

            if(loop.running.stage)
            {
                if(loop.running.stage->_dirty)
                {
                    _recompose = 0;
                    _ntg_stage_compose(loop.running.stage, loop.init.arena, &_recompose);
                    
                    if(!_recompose)
                        _ntg_stage_clean(loop.running.stage);
                    
                }
                drawing = &(loop.running.stage->_drawing);
            }
            else drawing = NULL;

            ntg_renderer_render(loop.init.renderer, drawing, loop.init.arena);
            nt_cursor_move(0, 0, NULL);

            sarena_rewind(loop.init.arena);
            // (loop.frame_count)++;
        }
        else
        {
            event_elapsed = (timeout >= event_elapsed) ? event_elapsed : timeout;
            timeout -= event_elapsed;
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_end);
        
        process_elapsed_ns = 
            ((int64_t)(ts_end.tv_sec - ts_start.tv_sec) * 1000000000LL) +
            ((int64_t)(ts_end.tv_nsec - ts_start.tv_nsec));
        
        process_elapsed_ms = process_elapsed_ns / 1000000LL;
        // loop.elapsed += (event_elapsed + process_elapsed_ms);
        timeout = (timeout > process_elapsed_ms) ? timeout - process_elapsed_ms : 0;
    }
}

/* ------------------------------------------------------ */
/* TASKS & PLATFORM */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_run_task(/* ??? */);

NTG_API bool
ntg_loop_has_running_tasks();

NTG_API void
ntg_loop_execute(/* ??? */);

/* ------------------------------------------------------ */
/* IN-LOOP */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_stop(int* out_status);

NTG_API ntg_stage*
ntg_loop_get_stage();

NTG_API void
ntg_loop_set_stage(ntg_stage* stage, int* out_status);

NTG_API struct ntg_xy
ntg_loop_get_app_size();

NTG_API unsigned int
ntg_loop_get_framerate();
