#include <assert.h>

#include "core/loop/ntg_loop.h"
#include "base/event/ntg_event.h"
#include "base/event/ntg_event_types.h"
#include "core/loop/ntg_taskmaster.h"
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

void __ntg_loop_init__(ntg_loop* loop,
        ntg_loop_process_event_fn process_event_fn,
        ntg_stage* init_stage,
        ntg_taskmaster* taskmaster,
        void* data)
{
    assert(loop != NULL);
    assert(init_stage != NULL);
    assert(taskmaster != NULL);

    loop->__process_event_fn = process_event_fn;
    loop->__init_stage = init_stage;
    loop->__taskmaster = taskmaster;
    loop->_data = data;
    loop->__delegate = ntg_event_delegate_new();
}

void __ntg_loop_deinit__(ntg_loop* loop)
{
    assert(loop != NULL);

    loop->__process_event_fn = NULL;
    ntg_event_delegate_destroy(loop->__delegate);
    loop->__delegate = NULL;
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
    struct ntg_loop_status status;
    unsigned int timeout = 0;
    struct nt_event _nt_event;
    ntg_event _ntg_event;

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
        // if(_status == NT_ERR_UNEXPECTED) break;

        status = loop->__process_event_fn(loop, &ctx, _nt_event);
        timeout = status.timeout;

        switch(_nt_event.type)
        {
            case NT_EVENT_KEY:
                key_data.key = _nt_event.key_data;
                __ntg_event_init__(
                        &_ntg_event,
                        NTG_EVENT_APP_KEYPRESS,
                        loop,
                        &key_data);
                ntg_event_delegate_raise(loop->__delegate, &_ntg_event);
                break;

            case NT_EVENT_RESIZE:
                resize_data.old = app_size;
                app_size = ntg_xy(
                        _nt_event.resize_data.width,
                        _nt_event.resize_data.height);
                ctx.app_size = app_size;
                resize_data.new = app_size;
                __ntg_event_init__(
                        &_ntg_event,
                        NTG_EVENT_APP_RESIZE,
                        loop,
                        &resize_data);
                ntg_event_delegate_raise(loop->__delegate, &_ntg_event);
                break;

            case NT_EVENT_TIMEOUT: // frame end
                ntg_taskmaster_execute_callbacks(loop->__taskmaster);
                break;
        }
    }

    __ntg_loop_ctx_deinit__(&ctx);
}

ntg_listenable* ntg_loop_get_listenable(ntg_loop* loop)
{
    assert(loop != NULL);

    return ntg_event_delegate_listenable(loop->__delegate);
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
