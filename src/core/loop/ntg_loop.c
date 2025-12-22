#include <time.h>
#include <assert.h>

#include "core/loop/ntg_loop.h"
#include "base/entity/ntg_event.h"
#include "base/entity/ntg_event_types.h"
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
    ntg_taskmaster* taskmaster;
    void* data;
};

void _ntg_loop_ctx_init_(ntg_loop_ctx* ctx,
        ntg_stage* init_stage, struct ntg_xy app_size,
        ntg_taskmaster* taskmaster, void* data);
static void _ntg_loop_ctx_deinit_(ntg_loop_ctx* ctx);

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

ntg_taskmaster* ntg_loop_ctx_get_taskmaster(ntg_loop_ctx* ctx)
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

static void _process_event_fn_def(ntg_loop* loop,
        ntg_loop_ctx* ctx, struct nt_event event);

void _ntg_loop_init_(ntg_loop* loop,
        struct ntg_loop_init_data loop_data,
        ntg_entity_group* group,
        ntg_entity_system* system)
{
    assert(loop != NULL);
    assert(loop_data.init_stage != NULL);
    assert(loop_data.taskmaster != NULL);
    assert(loop_data.renderer != NULL);

    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_TYPE_LOOP,
        .deinit_fn = _ntg_loop_deinit_fn,
        .group = group,
        .system = system
    };
    _ntg_entity_init_((ntg_entity*)loop, entity_data);

    if(loop_data.process_event_fn == NULL)
        loop->__process_event_fn = _process_event_fn_def;
    loop->__init_stage = loop_data.init_stage;
    loop->__renderer = loop_data.renderer;
    loop->__taskmaster = loop_data.taskmaster;
    loop->__framerate = loop_data.framerate;
    loop->data = NULL;
}

void _ntg_loop_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    _ntg_entity_deinit_fn(entity);

    ntg_loop* loop = (ntg_loop*)entity;

    loop->__process_event_fn = NULL;
    loop->__renderer = NULL;
    loop->data = NULL;
    loop->__framerate = 0;
    loop->__init_stage = NULL;
    loop->__taskmaster = NULL;
}

void ntg_loop_run(ntg_loop* loop, void* ctx_data)
{
    assert(loop != NULL);

    struct ntg_xy app_size;
    nt_get_term_size(&app_size.x, &app_size.y);

    ntg_loop_ctx ctx;
    _ntg_loop_ctx_init_(&ctx,
            loop->__init_stage,
            app_size,
            loop->__taskmaster,
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
                    // TODO:
                    // ntg_event_dlgt_raise(loop->__delegate,
                    //         NTG_EVENT_APP_KEYPRESS, loop, &key_data);
                    break;

                case NT_EVENT_RESIZE:
                    resize_data.old = app_size;
                    app_size = ntg_xy(
                            _nt_event.resize_data.width,
                            _nt_event.resize_data.height);
                    ctx.app_size = app_size;
                    resize_data.new = app_size;
                    // TODO:
                    // ntg_event_dlgt_raise(loop->__delegate,
                    //         NTG_EVENT_APP_RESIZE, loop, &resize_data);
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

    _ntg_loop_ctx_deinit_(&ctx);
}

/* -------------------------------------------------------------------------- */

void _ntg_loop_ctx_init_(ntg_loop_ctx* ctx,
        ntg_stage* init_stage, struct ntg_xy app_size,
        ntg_taskmaster* taskmaster, void* data)
{
    assert(ctx != NULL);
    assert(init_stage != NULL);

    ctx->pending_stage = init_stage;
    ctx->loop = true;
    ctx->app_size = app_size;
    ctx->taskmaster = taskmaster;
    ctx->data = data;
}

void _ntg_loop_ctx_deinit_(ntg_loop_ctx* ctx)
{
    assert(ctx != NULL);
    
    ctx->pending_stage = NULL;
    ctx->loop = false;
    ctx->app_size = ntg_xy(0, 0);
    ctx->taskmaster = NULL;
    ctx->data = NULL;
}

static void _process_event_fn_def(ntg_loop* loop,
        ntg_loop_ctx* ctx, struct nt_event event)
{
    if(event.type == NT_EVENT_KEY)
        ntg_stage_feed_key_event(ctx->stage, event.key_data, ctx);
}
