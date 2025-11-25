#include <assert.h>

#include "core/loop/ntg_loop.h"
#include "base/event/ntg_event.h"
#include "base/event/ntg_event_participants.h"
#include "base/event/ntg_event_types.h"
#include "nt.h"

struct ntg_loop_context
{
    ntg_stage *stage, *pending_stage;
    bool loop;
    struct ntg_xy app_size;
};

static void __ntg_loop_context_init__(ntg_loop_context* context,
        ntg_stage* init_stage, struct ntg_xy app_size);
static void __ntg_loop_context_deinit__(ntg_loop_context* context);

/* -------------------------------------------------------------------------- */

ntg_stage* ntg_loop_context_get_stage(ntg_loop_context* context)
{
    assert(context != NULL);

    return context->stage;
}

void ntg_loop_context_set_stage(ntg_loop_context* context, ntg_stage* stage)
{
    assert(context != NULL);

    context->pending_stage = stage;
}

void ntg_loop_context_break(ntg_loop_context* context)
{
    assert(context != NULL);

    context->loop = false;
}

struct ntg_xy ntg_loop_context_get_app_size(ntg_loop_context* context)
{
    assert(context != NULL);

    return context->app_size;
}

/* -------------------------------------------------------------------------- */

void __ntg_loop_init__(ntg_loop* loop,
        ntg_loop_process_event_fn loop_fn,
        void* data)
{
    assert(loop != NULL);
    assert(loop_fn != NULL);

    loop->__process_event_fn = loop_fn;
    loop->__delegate = ntg_event_delegate_new();
    loop->__data = data;
}

void __ntg_loop_deinit__(ntg_loop* loop)
{
    assert(loop != NULL);

    loop->__process_event_fn = NULL;
    ntg_event_delegate_destroy(loop->__delegate);
    loop->__delegate = NULL;
}

void ntg_loop_run(ntg_loop* loop, ntg_stage* init_stage, unsigned int init_timeout)
{
    assert(loop != NULL);

    struct ntg_xy app_size;
    nt_get_term_size(&app_size.x, &app_size.y);

    ntg_loop_context context;
    __ntg_loop_context_init__(&context, init_stage, app_size);

    /* loop */
    struct ntg_loop_status status;
    unsigned int timeout = init_timeout;
    struct nt_event _nt_event;
    ntg_event _ntg_event;

    /* loop resize */
    struct ntg_event_app_resize_data resize_data;

    /* loop key */
    struct ntg_event_app_key_data key_data;
    nt_status _status;
    while(true)
    {
        if(!context.loop) break;
        if(context.pending_stage != NULL)
        {
            context.stage = context.pending_stage;
            context.pending_stage = NULL;
        }

        _nt_event = nt_wait_for_event(timeout, &_status);
        // if(_status == NT_ERR_UNEXPECTED) break;

        status = loop->__process_event_fn(loop, &context, _nt_event);
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
                context.app_size = app_size;
                resize_data.new = app_size;
                __ntg_event_init__(
                        &_ntg_event,
                        NTG_EVENT_APP_RESIZE,
                        loop,
                        &resize_data);
                ntg_event_delegate_raise(loop->__delegate, &_ntg_event);
                break;

            case NT_EVENT_TIMEOUT:
                break;
        }
    }

    __ntg_loop_context_deinit__(&context);
}

ntg_listenable* ntg_loop_get_listenable(ntg_loop* loop)
{
    assert(loop != NULL);

    return ntg_event_delegate_listenable(loop->__delegate);
}

/* -------------------------------------------------------------------------- */

void __ntg_loop_context_init__(ntg_loop_context* context,
        ntg_stage* init_stage, struct ntg_xy app_size)
{
    assert(context != NULL);
    assert(init_stage != NULL);
    
    context->pending_stage = init_stage;
    context->loop = true;
    context->app_size = app_size;
}

void __ntg_loop_context_deinit__(ntg_loop_context* context)
{
    assert(context != NULL);
    
    context->pending_stage = NULL;
    context->loop = false;
    context->app_size = ntg_xy(0, 0);
}
