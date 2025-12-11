#include <limits.h>
#include <stdlib.h>
#include <assert.h>

#include "base/event/ntg_event.h"
#include "base/event/_ntg_event_sub_vec.h"

static unsigned int __id_gen = 0;

struct ntg_listenable
{
    ntg_event_sub_vec* __subs;
};

struct ntg_event_dlgt
{
    ntg_event_sub_vec* __subs;
};

void ntg_listenable_listen(ntg_listenable* listenable,
        void* subscriber, ntg_event_handler handler)
{
    assert(listenable != NULL);
    assert(subscriber != NULL);
    assert(handler != NULL);

    if(!ntg_listenable_has_sub(listenable, subscriber, handler))
    {
        ntg_event_sub_vec_append(listenable->__subs, subscriber, handler);
    }
}

void ntg_listenable_stop_listening(ntg_listenable* listenable,
        void* subscriber, ntg_event_handler handler)
{
    assert(listenable != NULL);

    if(ntg_listenable_has_sub(listenable, subscriber, handler))
    {
        ntg_event_sub_vec_remove(listenable->__subs, subscriber, handler);
    }
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

ntg_event_dlgt* ntg_event_dlgt_new()
{
    ntg_event_dlgt* new = (ntg_event_dlgt*)malloc(sizeof(ntg_event_dlgt));
    assert(new != NULL);

    new->__subs = ntg_event_sub_vec_new();
    
    return new;
}

void ntg_event_dlgt_destroy(ntg_event_dlgt* event_dlgt)
{
    assert(event_dlgt != NULL);

    ntg_event_sub_vec_destroy(event_dlgt->__subs);
    free(event_dlgt);
}

ntg_listenable* ntg_event_dlgt_listenable(ntg_event_dlgt* event_dlgt)
{
    assert(event_dlgt != NULL);

    return (ntg_listenable*)event_dlgt;
}

void ntg_event_dlgt_raise(ntg_event_dlgt* event_dlgt, unsigned int type,
        void* source, void* data)
{
    assert(event_dlgt != NULL);
    assert(type != NTG_EVENT_TYPE_INVALID);
    assert(source != NULL);

    ntg_event_sub_vec* subs = event_dlgt->__subs;

    struct ntg_event event = {
        .id = __id_gen,
        .type = type,
        .source = source,
        .data = data
    };

    size_t i;
    for(i = 0; i < subs->_count; i++)
    {
        subs->_data[i].handler(subs->_data[i].subscriber, event);
    }
}
