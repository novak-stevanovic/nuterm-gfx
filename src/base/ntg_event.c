#include <limits.h>
#include <stdlib.h>
#include <assert.h>

#include "base/ntg_event.h"
#include "base/ntg_event_sub_vec.h"

static uint __id_gen = 0;

void __ntg_event_init__(ntg_event* event, uint type, void* source, void* data)
{
    assert(event != NULL);

    event->_type = type;
    event->_source = source;
    event->_data = data;
    event->_id = __id_gen;
    __id_gen++;
}

void __ntg_event_deinit__(ntg_event* event)
{
    event->_type = UINT_MAX;
    event->_id = UINT_MAX;
    event->_data = NULL;
    event->_source = NULL;
}

/* -------------------------------------------------------------------------- */

void __ntg_event_delegate_init__(ntg_event_delegate* delegate)
{
    assert(delegate != NULL);
    
    __ntg_event_sub_vec_init__(delegate->_subs);
}

void __ntg_event_delegate_deinit__(ntg_event_delegate* delegate)
{
    assert(delegate != NULL);

    __ntg_event_sub_vec_deinit__(delegate->_subs);
}

void ntg_event_delegate_raise(ntg_event_delegate* delegate, ntg_event* event)
{
    assert(event != NULL);

    size_t i;
    struct ntg_event_sub it_sub;
    for(i = 0; i < delegate->_subs->_count; i++)
    {
        it_sub = delegate->_subs->_data[i];
        it_sub.handler(it_sub.subscriber, event);
    }
}

/* -------------------------------------------------------------------------- */

void __ntg_event_delegate_view_init__(ntg_event_delegate_view* view,
        ntg_event_delegate* delegate)
{
    assert(view != NULL);

    view->__delegate = delegate;
}

void ntg_event_delegate_view_sub(ntg_event_delegate_view* view,
        struct ntg_event_sub subscription)
{
    assert(view != NULL);
    assert(subscription.subscriber != NULL);
    assert(subscription.handler != NULL);

    ntg_event_sub_vec_append(view->__delegate->_subs, subscription);
}

void ntg_event_delegate_view_unsub(ntg_event_delegate_view* view,
        void* subscriber)
{
    assert(view != NULL);
    assert(subscriber != NULL);

    ssize_t find_status = ntg_event_sub_vec_find_sub(
            view->__delegate->_subs,
            subscriber);

    if(find_status != -1)
    {
        ntg_event_sub_vec_remove_sub(view->__delegate->_subs, subscriber);
    }
}

bool ntg_event_delegate_view_is_subbed(const ntg_event_delegate_view* view,
        void* subscriber)
{
    assert(view != NULL);

    ssize_t find_status = ntg_event_sub_vec_find_sub(
            view->__delegate->_subs,
            subscriber);

    assert(find_status != -2);

    return (find_status != -1);
}
