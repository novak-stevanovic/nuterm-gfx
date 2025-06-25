#include "base/ntg_event.h"
#include "base/ntg_event_sub_vec.h"
#include <assert.h>
#include <stdlib.h>

static uint _id = 1;

static void _ntg_event_sub_delegate_raise(ntg_event_sub_delegate* sub_delegate,
        ntg_event* event);

void __ntg_event_init__(ntg_event* event, uint type, void* source, void* data)
{
    assert(event != NULL);

    event->_id = _id;
    _id++;

    event->_type = type;
    event->_source = source;
    assert(source != NULL); // TODO: ?
    event->_data = data;
}
void __ntg_event_deinit__(ntg_event* event)
{
    (*event) = (ntg_event) {0};
}

/* -------------------------------------------------------------------------- */

void __ntg_event_sub_delegate_init__(ntg_event_sub_delegate* sub_delegate)
{
    assert(sub_delegate != NULL);

    __ntg_event_sub_vec_init__(&sub_delegate->__subs);
}

void __ntg_event_sub_delegate_deinit__(ntg_event_sub_delegate* sub_delegate)
{
    assert(sub_delegate != NULL);

    __ntg_event_sub_vec_deinit__(&sub_delegate->__subs);
}

void ntg_event_sub_delegate_sub(ntg_event_sub_delegate* sub_delegate,
        struct ntg_event_sub subscription)

{
    assert(sub_delegate != NULL);

    ntg_event_sub_vec_append(&sub_delegate->__subs, subscription);
}

void ntg_event_sub_delegate_unsub(ntg_event_sub_delegate* sub_delegate,
        void* subscriber)
{
    ntg_event_sub_vec_remove_sub(&sub_delegate->__subs, subscriber);
}

bool ntg_event_sub_delegate_is_sub(ntg_event_sub_delegate* sub_delegate,
        void* subscriber)
{
    ssize_t status = ntg_event_sub_vec_find_sub(&sub_delegate->__subs, subscriber);

    return (status != -1);
}

/* -------------------------------------------------------------------------- */

void __ntg_event_delegate_init__(ntg_event_delegate* delegate)
{
    assert(delegate != NULL);

    __ntg_event_sub_delegate_init__(&delegate->_sub_delegate);
}

void __ntg_event_delegate_deinit__(ntg_event_delegate* delegate)
{
    assert(delegate != NULL);

    __ntg_event_sub_delegate_deinit__(&delegate->_sub_delegate);
}

void ntg_event_delegate_raise(ntg_event_delegate* delegate, ntg_event* event)
{
    _ntg_event_sub_delegate_raise(&delegate->_sub_delegate, event);
}

static void _ntg_event_sub_delegate_raise(ntg_event_sub_delegate* sub_delegate,
        ntg_event* event)
{
    assert(sub_delegate != NULL);

    size_t i;
    struct ntg_event_sub* it_sub;
    for(i = 0; i < sub_delegate->__subs._count; i++)
    {
        it_sub = &(sub_delegate->__subs._data[i]);

        it_sub->handler(it_sub->subscriber, event);
    }
}
