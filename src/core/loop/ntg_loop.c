#include "ntg.h"
#include <assert.h>
#include "nt.h"
#include <time.h>
#include <signal.h>

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_loop_run(ntg_loop* loop, struct ntg_loop_run_data data)
{
    assert(loop != NULL);

    assert(data.renderer != NULL);
    assert(data.stage != NULL);
    if(data.framerate > 300) data.framerate = 300;


    ntg_loop_ctx ctx;
    ctx._stage = data.stage;
    nt_get_term_size(&ctx._app_size.x, &ctx._app_size.y);
    ctx.__loop = true;
    ctx._elapsed = 0;
    ctx._frame = 0;
    ctx.data = data.ctx_data;
    ctx._arena = sarena_create(50000);

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

    sarena_destroy(ctx._arena);
    ctx = (struct ntg_loop_ctx) {0};
}

void ntg_loop_ctx_break(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);

    ctx->__loop = false;
}
