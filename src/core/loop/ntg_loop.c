#include <time.h>
#include <assert.h>

#include "core/loop/ntg_loop.h"
#include "base/event/ntg_event.h"
#include "base/event/ntg_event_types.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/renderer/ntg_renderer.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/stage/ntg_stage.h"
#include "nt.h"

struct ntg_loop_ctx
{
    ntg_stage *stage, *pending_stage;
    bool loop;
    struct ntg_xy app_size;
    ntg_taskmaster_channel* taskmaster;
    void* data;
};

void __ntg_loop_ctx_init__(ntg_loop_ctx* ctx,
        ntg_stage* init_stage, struct ntg_xy app_size,
        ntg_taskmaster_channel* taskmaster, void* data);
static void __ntg_loop_ctx_deinit__(ntg_loop_ctx* ctx);

/* -------------------------------------------------------------------------- */

ntg_stage* ntg_loop_ctx_get_stage(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);

    return ctx->stage;
}

void ntg_loop_ctx_set_stage(ntg_loop_ctx* ctx, ntg_stage* stage)
{
    assert(ctx != NULL);

    ctx->pending_stage = stage;
}

void ntg_loop_ctx_break(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);

    ctx->loop = false;
}

struct ntg_xy ntg_loop_ctx_get_app_size(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);

    return ctx->app_size;
}

ntg_taskmaster_channel* ntg_loop_ctx_get_taskmaster(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);

    return ctx->taskmaster;
}

void* ntg_loop_ctx_get_data(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);

    return ctx->data;
}

/* -------------------------------------------------------------------------- */

static void __process_event_fn_def(ntg_loop* loop,
        ntg_loop_ctx* ctx, struct nt_event event);

void __ntg_loop_init__(ntg_loop* loop,
        ntg_loop_process_event_fn process_event_fn,
        ntg_stage* init_stage,
        ntg_renderer* renderer,
        ntg_taskmaster* taskmaster,
        unsigned int framerate,
        void* data)
{
    assert(loop != NULL);
    assert(init_stage != NULL);
    assert(taskmaster != NULL);

    loop->__process_event_fn = (process_event_fn != NULL) ?
        process_event_fn : __process_event_fn_def;
    loop->__init_stage = init_stage;
    loop->__renderer = renderer;
    loop->__taskmaster = taskmaster;
    loop->__framerate = framerate;
    loop->_data = data;
    loop->__delegate = ntg_event_dlgt_new();
}

void __ntg_loop_deinit__(ntg_loop* loop)
{
    assert(loop != NULL);

    loop->__process_event_fn = NULL;
    ntg_event_dlgt_destroy(loop->__delegate);
    loop->__delegate = NULL;
    loop->__renderer = NULL;
    loop->_data = NULL;
    loop->__framerate = 0;
}

void ntg_loop_run(ntg_loop* loop, void* ctx_data)
{
    assert(loop != NULL);

    struct ntg_xy app_size;
    nt_get_term_size(&app_size.x, &app_size.y);

    ntg_loop_ctx ctx;
    __ntg_loop_ctx_init__(&ctx,
            loop->__init_stage,
            app_size,
            ntg_taskmaster_get_channel(loop->__taskmaster),
            ctx_data);

    /* loop */
    unsigned int timeout = 1000 / loop->__framerate;
    struct timespec ts_start, ts_end;
    int64_t process_elapsed_ns;
    uint64_t process_elapsed_ms;
    const ntg_stage_drawing* drawing;

    struct nt_event _nt_event;

    /* loop resize */
    struct ntg_event_app_resize_data resize_data;

    /* loop key */
    struct ntg_event_app_key_data key_data;
    nt_status _status;
    while(true)
    {
        if(!ctx.loop) break;
        if(ctx.pending_stage != NULL)
        {
            ctx.stage = ctx.pending_stage;
            ctx.pending_stage = NULL;
        }

        _nt_event = nt_wait_for_event(timeout, &_status);
        clock_gettime(CLOCK_MONOTONIC, &ts_start);
        // if(_status == NT_ERR_UNEXPECTED) break;

        loop->__process_event_fn(loop, &ctx, _nt_event);

        if(_nt_event.type == NT_EVENT_TIMEOUT)
        {
            timeout = 1000 / loop->__framerate;

            if(ctx.stage != NULL)
            {
                ntg_stage_compose(ctx.stage, app_size);
                drawing = ntg_stage_get_drawing(ctx.stage);
            }
            else drawing = NULL;

            ntg_renderer_render(loop->__renderer, drawing, app_size);
            ntg_taskmaster_execute_callbacks(loop->__taskmaster);
        }
        else
        {
            timeout -= _nt_event.elapsed;
            switch(_nt_event.type)
            {
                case NT_EVENT_KEY:
                    key_data.key = _nt_event.key_data;
                    ntg_event_dlgt_raise(loop->__delegate,
                            NTG_EVENT_APP_KEYPRESS, loop, &key_data);
                    break;

                case NT_EVENT_RESIZE:
                    resize_data.old = app_size;
                    app_size = ntg_xy(
                            _nt_event.resize_data.width,
                            _nt_event.resize_data.height);
                    ctx.app_size = app_size;
                    resize_data.new = app_size;
                    ntg_event_dlgt_raise(loop->__delegate,
                            NTG_EVENT_APP_RESIZE, loop, &resize_data);
                    break;
                case NT_EVENT_TIMEOUT: assert(0);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &ts_end);

        process_elapsed_ns = (int64_t)(ts_end.tv_sec - ts_start.tv_sec) * 1000000000LL
            + (int64_t)(ts_end.tv_nsec - ts_start.tv_nsec);
        process_elapsed_ns = (process_elapsed_ns > 0) ? process_elapsed_ns : 0;

        process_elapsed_ms = process_elapsed_ns / 1000000LL;
        timeout = (timeout > process_elapsed_ms) ? timeout - process_elapsed_ms : 0;
    }

    __ntg_loop_ctx_deinit__(&ctx);
}

ntg_listenable* ntg_loop_get_listenable(ntg_loop* loop)
{
    assert(loop != NULL);

    return ntg_event_dlgt_listenable(loop->__delegate);
}

/* -------------------------------------------------------------------------- */

void __ntg_loop_ctx_init__(ntg_loop_ctx* ctx,
        ntg_stage* init_stage, struct ntg_xy app_size,
        ntg_taskmaster_channel* taskmaster, void* data)
{
    assert(ctx != NULL);
    assert(init_stage != NULL);

    ctx->pending_stage = init_stage;
    ctx->loop = true;
    ctx->app_size = app_size;
    ctx->taskmaster = taskmaster;
    ctx->data = data;
}

void __ntg_loop_ctx_deinit__(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);
    
    ctx->pending_stage = NULL;
    ctx->loop = false;
    ctx->app_size = ntg_xy(0, 0);
    ctx->taskmaster = NULL;
    ctx->data = NULL;
}

static void __process_event_fn_def(ntg_loop* loop,
        ntg_loop_ctx* ctx, struct nt_event event)
{
    if(event.type == NT_EVENT_KEY)
        ntg_stage_feed_key_event(ctx->stage, event.key_data, ctx);
}
