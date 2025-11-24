#include <assert.h>

#include "core/loop/ntg_loop.h"
#include "base/event/ntg_event.h"
#include "base/event/ntg_event_participants.h"
#include "core/loop/ntg_loop_context.h"
#include "base/event/ntg_event_types.h"
#include "nt.h"

void __ntg_loop_init__(ntg_loop* loop,
        ntg_loop_create_context_fn create_context_fn,
        ntg_loop_destroy_context_fn destroy_context_fn,
        ntg_loop_fn loop_fn,
        ntg_stage* init_stage,
        void* data)
{
    assert(loop != NULL);
    assert(loop_fn != NULL);
    assert(create_context_fn != NULL);
    assert(destroy_context_fn != NULL);

    loop->__loop_fn = loop_fn;
    loop->__init_stage = init_stage;
    loop->__create_context_fn = create_context_fn;
    loop->__destroy_context_fn = destroy_context_fn;
    loop->__delegate = ntg_event_delegate_new();
    loop->__data = data;
}

void __ntg_loop_deinit__(ntg_loop* loop)
{
    assert(loop != NULL);

    loop->__loop_fn = NULL;
    ntg_event_delegate_destroy(loop->__delegate);
    loop->__delegate = NULL;
}

void ntg_loop_run(ntg_loop* loop, void* context_data)
{
    assert(loop != NULL);

    ntg_loop_context* context = loop->__create_context_fn(loop, context_data);

    /* loop */
    struct ntg_loop_status it_status;
    ntg_event event;

    /* loop resize */
    size_t _width, _height;
    struct ntg_event_app_resize_data resize_data;

    /* loop key */
    struct ntg_event_app_key_data key_data;
    while(_ntg_loop_context_update(context))
    {
        nt_get_term_size(&_width, &_height); 

        it_status = loop->__loop_fn(context);

        switch(it_status.event.type)
        {
            case NT_EVENT_KEY:
                key_data.key = it_status.event.key_data;
                __ntg_event_init__(&event, NTG_EVENT_APP_KEYPRESS,
                        NTG_EVENT_PARTICIPANT_APP, &key_data);
                ntg_event_delegate_raise(loop->__delegate, &event);
                break;

            case NT_EVENT_RESIZE:
                resize_data.old = ntg_xy(_width, _height);
                resize_data.new = ntg_xy(it_status.event.resize_data.width,
                        it_status.event.resize_data.height);
                __ntg_event_init__(&event, NTG_EVENT_APP_RESIZE,
                        NTG_EVENT_PARTICIPANT_APP, &resize_data);
                ntg_event_delegate_raise(loop->__delegate, &event);
                break;

            case NT_EVENT_TIMEOUT:
                break;
        }
    }

    loop->__destroy_context_fn(context);
}

ntg_listenable* ntg_loop_get_listenable(ntg_loop* loop)
{
    assert(loop != NULL);

    return ntg_event_delegate_listenable(loop->__delegate);
}
