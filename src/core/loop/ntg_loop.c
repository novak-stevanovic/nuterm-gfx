#include <time.h>
#include <signal.h>
#include <assert.h>
#include "ntg.h"
#include "nt.h"
#include "core/loop/_ntg_loop.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

bool ntg_loop_dispatch_def(ntg_loop* loop, struct nt_event event,
                           ntg_loop_ctx* ctx)
{
    if(ctx->_stage)
    {
        struct ntg_event dispatch_event;

        if(event.type == NT_EVENT_KEY)
        {
            struct nt_key_event key = *(struct nt_key_event*)event.data;
            struct ntg_event_loop_key_data data = { .key = key };
            dispatch_event = ntg_event_new((ntg_entity*)loop, NTG_EVENT_LOOP_KEY,
                                           &data);

            return ntg_stage_feed_event(ctx->_stage, dispatch_event, ctx);
        }
        else if(event.type == NT_EVENT_MOUSE)
        {
            struct nt_mouse_event mouse = *(struct nt_mouse_event*)event.data;
            struct ntg_event_loop_mouse_data data = { .mouse = mouse };
            dispatch_event = ntg_event_new((ntg_entity*)loop, NTG_EVENT_LOOP_MOUSE,
                                           &data);

            return ntg_stage_feed_event(ctx->_stage, dispatch_event, ctx);
        }
        else return false;
    }
    else return false;
}

enum ntg_loop_status 
ntg_loop_run(ntg_loop* loop, struct ntg_loop_run_data data)
{
    assert(loop != NULL);

    assert(data.renderer != NULL);
    assert(data.stage != NULL);
    assert(data.workers <= NTG_LOOP_WORKERS_MAX);
    if(!data.process_fn) data.process_fn = ntg_loop_dispatch_def;
    if(data.framerate > 300) data.framerate = 300;

    ntg_platform platform;
    _ntg_platform_init(&platform);
    ntg_task_runner task_runner;
    _ntg_task_runner_init(&task_runner, &platform, data.workers);

    ntg_loop_ctx ctx;
    ctx._stage = data.stage;
    nt_get_term_size(&ctx._app_size.x, &ctx._app_size.y);
    ctx.__loop = true;
    ctx.__force_break = false;
    ctx._elapsed = 0;
    ctx._frame = 0;
    ctx._arena = sarena_create(1000000);
    ctx._platform = &platform;
    ctx._task_runner = &task_runner;
    ctx.data = data.ctx_data;

    size_t resize_counter = 0, sigwinch_counter = 0;

    /* loop */
    unsigned int timeout = 1000 / data.framerate;
    struct timespec ts_start, ts_end;
    int64_t process_elapsed_ns;
    uint64_t process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event event;
    struct nt_resize_event resize_event;
    unsigned int event_elapsed;
    struct ntg_event_loop_generic_data loop_event_data;

    nt_status _status;

    while(true)
    {
        if(!(ctx.__loop)) break;

        event_elapsed = nt_event_wait(&event, timeout, &_status);
        assert(_status == NT_SUCCESS);
        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        if(event.type == NT_EVENT_RESIZE)
        {
            resize_event = *(struct nt_resize_event*)event.data;
            ctx._app_size.x = resize_event.new_x;
            ctx._app_size.y = resize_event.new_y;

            resize_counter++;
        }
        if(event.type == NT_EVENT_SIGNAL)
        {
            sigwinch_counter++;
        }

        data.process_fn(loop, event, &ctx);

        // Frame end
        if(event.type == NT_EVENT_TIMEOUT)
        {
            _ntg_platform_execute_all(&platform, &ctx);

            timeout = 1000 / data.framerate;

            if(ctx._stage != NULL)
            {
                ntg_stage_compose(ctx._stage, ctx._app_size, ctx._arena);
                drawing = &ctx._stage->_drawing;
            }
            else drawing = NULL;

            ntg_renderer_render(data.renderer, drawing, ctx._arena);

            sarena_rewind(ctx._arena);
            ctx._frame++;
        }
        else
        {
            timeout -= event_elapsed;
        }

        // TODO
        loop_event_data.event = event;
        loop_event_data.ctx = &ctx;
        ntg_entity_raise_event_((ntg_entity*)loop, NTG_EVENT_LOOP_GENERIC,
                               &loop_event_data);

        clock_gettime(CLOCK_MONOTONIC, &ts_end);

        // Subtract time taken inside the loop iteration
        process_elapsed_ns = (int64_t)(ts_end.tv_sec - ts_start.tv_sec) * 1000000000LL
            + (int64_t)(ts_end.tv_nsec - ts_start.tv_nsec);
        // process_elapsed_ns = (process_elapsed_ns > 0) ? process_elapsed_ns : 0;

        process_elapsed_ms = process_elapsed_ns / 1000000LL;
        ctx._elapsed += (event_elapsed + process_elapsed_ms);
        timeout = (timeout > process_elapsed_ms) ? timeout - process_elapsed_ms : 0;
    }

    enum ntg_loop_status status;
    if(!ctx.__force_break)
    {
        status = NTG_LOOP_FINISH_CLEAN;
        _ntg_platform_deinit(&platform);
        _ntg_task_runner_deinit(&task_runner);
    }
    else
    {
        status = NTG_LOOP_FINISH_FORCE;
        _ntg_platform_invalidate(&platform);
        _ntg_task_runner_invalidate(&task_runner);
    }

    ntg_log_log("%d %d", resize_counter, sigwinch_counter);

    sarena_destroy(ctx._arena);
    // ctx = (struct ntg_loop_ctx) {0};
    return status;
}

bool ntg_loop_ctx_break(ntg_loop_ctx* ctx, bool force_break)
{
    assert(ctx != NULL);

    if(force_break)
    {
        ctx->__loop = false;
        ctx->__force_break = true;

        return true;
    }
    else
    {
        if(!_ntg_task_runner_is_running(ctx->_task_runner))
        {
            ctx->__loop = false;
            return true;
        }

        return false;
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
        void (*task_fn)(void* data, ntg_loop_ctx* ctx),
        void* data)
{
    struct ntg_ptask task = {
        .task_fn = task_fn,
        .data = data
    };
    _ntg_platform_execute_later(platform, task);
}

