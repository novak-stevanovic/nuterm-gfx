#include <time.h>
#include <signal.h>
#include <assert.h>
#include "ntg.h"
#include "nt.h"
#include "core/loop/_ntg_loop.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

enum ntg_loop_status ntg_loop_run(ntg_loop* loop, struct ntg_loop_run_data data)
{
    assert(loop != NULL);

    assert(data.renderer != NULL);
    assert(data.stage != NULL);
    assert(data.worker_threads <= NTG_LOOP_WORKER_THREADS_MAX);
    if(data.framerate > 300) data.framerate = 300;

    ntg_platform platform;
    _ntg_platform_init(&platform);
    ntg_task_runner task_runner;
    _ntg_task_runner_init(&task_runner, &platform, data.worker_threads);

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

    /* loop */
    unsigned int timeout = 1000 / data.framerate;
    struct timespec ts_start, ts_end;
    int64_t process_elapsed_ns;
    uint64_t process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event event;
    unsigned int event_elapsed;
    struct ntg_event_loop_event_data loop_event_data;

    nt_status _status;

    while(true)
    {
        if(!(ctx.__loop)) break;

        event_elapsed = nt_event_wait(&event, timeout, &_status);
        assert(_status == NT_SUCCESS);
        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        if(event.type == NT_EVENT_SIGNAL)
        {
            unsigned int signum = *(unsigned int*)event.data;
            if(signum == SIGWINCH) // RESIZE
            {
                nt_get_term_size(&ctx._app_size.x, &ctx._app_size.y);
            }
        }

        // Process/dispatch event
        bool consumed = false;
        if(data.event_mode == NTG_LOOP_EVENT_PROCESS_FIRST)
        {
            if(data.event_fn != NULL)
                consumed = data.event_fn(&ctx, event);

            if((!consumed) && (data.stage != NULL))
                ntg_stage_feed_event(data.stage, event, &ctx);
        }
        else // data.event_mode = NTG_LOOP_EVENT_DISPATCH_FIRST
        {
            if(data.stage != NULL)
                consumed = ntg_stage_feed_event(data.stage, event, &ctx);

            if((!consumed) && (data.event_fn != NULL))
                data.event_fn(&ctx, event);
        }

        // Frame end
        if(event.type == NT_EVENT_TIMEOUT)
        {
            _ntg_platform_execute_all(&platform, &ctx);

            timeout = 1000 / data.framerate;

            if(ctx._stage != NULL)
            {
                ntg_stage_compose(ctx._stage, ctx._app_size, ctx._arena);
                drawing = ntg_stage_get_drawing(ctx._stage);
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

        loop_event_data.event = event;
        ntg_entity_raise_event((ntg_entity*)loop, NULL,
                NTG_EVENT_LOOP_EVENT, &loop_event_data);

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
        status = NTG_LOOP_CLEAN_FINISH;
        _ntg_platform_deinit(&platform);
        _ntg_task_runner_deinit(&task_runner);
    }
    else
    {
        status = NTG_LOOP_FORCE_FINISH;
        _ntg_platform_invalidate(&platform);
        _ntg_task_runner_invalidate(&task_runner);
    }

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
