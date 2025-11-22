#include <limits.h>
#include <stdlib.h>
#include <assert.h>

#include "base/event/ntg_event.h"
#include "base/event/_ntg_event_sub_vec.h"

static unsigned int __id_gen = 0;

void __ntg_event_init__(ntg_event* event, unsigned int type,
        void* source, void* data)
{
    assert(event != NULL);

    event->_type = type;
    event->_source = source;
    event->_data = data;
    event->_id = __id_gen;
    __id_gen++;
}

struct ntg_listenable
{
    ntg_event_sub_vec* __subs;
};

struct ntg_event_delegate
{
    ntg_event_sub_vec* __subs;
};

void ntg_listenable_listen(ntg_listenable* listenable,
        void* subscriber, ntg_event_handler handler)
{
    assert(listenable != NULL);
    assert(subscriber != NULL);
    assert(handler != NULL);

    assert(!ntg_listenable_has_sub(listenable, subscriber, handler));

    ntg_event_sub_vec_append(listenable->__subs, subscriber, handler);
}

void ntg_listenable_stop_listening(ntg_listenable* listenable,
        void* subscriber, ntg_event_handler handler)
{
    assert(listenable != NULL);

    assert(ntg_listenable_has_sub(listenable, subscriber, handler));

    ntg_event_sub_vec_remove(listenable->__subs, subscriber, handler);
}

bool ntg_listenable_is_listening(ntg_listenable* listenable, void* subscriber)
{
    assert(listenable != NULL);
    assert(subscriber != NULL);

    return ntg_event_sub_vec_find(listenable->__subs, subscriber);
}

bool ntg_listenable_has_sub(ntg_listenable* listenable,
        void* subscriber, ntg_event_handler handler)
{
    assert(listenable != NULL);
    assert(subscriber != NULL);

    return ntg_event_sub_vec_contains_(listenable->__subs, subscriber, handler);
}

ntg_event_delegate* ntg_event_delegate_new()
{
    ntg_event_delegate* new = (ntg_event_delegate*)malloc(sizeof(ntg_event_delegate));
    assert(new != NULL);

    new->__subs = ntg_event_sub_vec_new();
    
    return new;
}

void ntg_event_delegate_destroy(ntg_event_delegate* del)
{
    assert(del != NULL);

    ntg_event_sub_vec_destroy(del->__subs);
    free(del);
}

ntg_listenable* ntg_event_delegate_listenable(ntg_event_delegate* del)
{
    assert(del != NULL);

    return (ntg_listenable*)del;
}

void ntg_event_delegate_raise(ntg_event_delegate* del, ntg_event* event)
{
    assert(del != NULL);

    ntg_event_sub_vec* subs = del->__subs;

    size_t i;
    for(i = 0; i < subs->_count; i++)
    {
        subs->_data[i].handler(subs->_data[i].subscriber, event);
    }
}
