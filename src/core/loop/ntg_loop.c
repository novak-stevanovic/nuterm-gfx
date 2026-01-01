#include <time.h>
#include <assert.h>

#include "core/loop/ntg_loop.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/renderer/ntg_renderer.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/stage/ntg_stage.h"
#include "nt.h"
#include "shared/ntg_log.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_loop_run(ntg_loop* loop, struct ntg_loop_run_data data)
{
    assert(loop != NULL);

    assert(data.taskmaster != NULL);
    assert(data.renderer != NULL);
    assert(data.stage != NULL);
    if(data.framerate > 300) data.framerate = 300;

    struct ntg_xy app_size;
    nt_get_term_size(&app_size.x, &app_size.y);

    ntg_loop_ctx ctx;
    ctx._stage = data.stage;
    ctx._taskmaster = data.taskmaster;
    ctx._app_size = app_size;
    ctx.__loop = true;
    ctx._elapsed = 0;
    ctx._frame = 0;
    ctx.data = data.ctx_data;

    /* loop */
    unsigned int timeout = 1000 / data.framerate;
    struct timespec ts_start, ts_end;
    int64_t process_elapsed_ns;
    uint64_t process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event event;

    /* loop resize */
    struct ntg_event_loop_rsz_data resize_data;

    /* loop key */
    struct ntg_event_loop_key_data key_data;
    nt_status _status;
    while(true)
    {
        if(!(ctx.__loop)) break;

        event = nt_wait_for_event(timeout, &_status);
        clock_gettime(CLOCK_MONOTONIC, &ts_start);

        // Process/dispatch event
        struct ntg_loop_event loop_event = { .event = event };
        bool consumed = false;
        if(data.event_mode == NTG_LOOP_EVENT_PROCESS_FIRST)
        {
            if(data.event_fn != NULL)
                consumed = data.event_fn(&ctx, loop_event);

            if((!consumed) && (data.stage != NULL))
                ntg_stage_feed_event(data.stage, loop_event, &ctx);
        }
        else // data.event_mode = NTG_LOOP_EVENT_DISPATCH_FIRST
        {
            if(data.stage != NULL)
                consumed = ntg_stage_feed_event(data.stage, loop_event, &ctx);

            if((!consumed) && (data.event_fn != NULL))
                data.event_fn(&ctx, loop_event);
        }

        // Default handling of events
        if(event.type == NT_EVENT_TIMEOUT)
        {
            timeout = 1000 / data.framerate;

            if(ctx._stage != NULL)
            {
                ntg_stage_compose(ctx._stage, app_size);
                drawing = ntg_stage_get_drawing(ctx._stage);
            }
            else drawing = NULL;

            ntg_renderer_render(data.renderer, drawing);
            ntg_taskmaster_execute_callbacks(data.taskmaster);

            ctx._frame++;
        }
        else
        {
            timeout -= event.elapsed;
            switch(event.type)
            {
                case NT_EVENT_KEY:
                    key_data.key = event.key_data;
                    ntg_entity_raise_event((ntg_entity*)loop, NULL,
                            NTG_EVENT_LOOP_KEY, &key_data);
                    break;

                case NT_EVENT_RESIZE:
                    resize_data.old = app_size;
                    app_size = ntg_xy(
                            event.resize_data.width,
                            event.resize_data.height);
                    ctx._app_size = app_size;
                    resize_data.new = app_size;
                    ntg_entity_raise_event((ntg_entity*)loop, NULL,
                            NTG_EVENT_LOOP_RSZ, &resize_data);
                    break;
                case NT_EVENT_TIMEOUT: assert(0);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_end);

        // Subtract time taken inside the loop iteration
        process_elapsed_ns = (int64_t)(ts_end.tv_sec - ts_start.tv_sec) * 1000000000LL
            + (int64_t)(ts_end.tv_nsec - ts_start.tv_nsec);
        // process_elapsed_ns = (process_elapsed_ns > 0) ? process_elapsed_ns : 0;

        process_elapsed_ms = process_elapsed_ns / 1000000LL;
        timeout = (timeout > process_elapsed_ms) ? timeout - process_elapsed_ms : 0;
        ntg_log_log("FRAME PROCESS TIME: %d ms", process_elapsed_ms);
    }

    ctx = (struct ntg_loop_ctx) {0};
}

void ntg_loop_ctx_break(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);

    ctx->__loop = false;
}
