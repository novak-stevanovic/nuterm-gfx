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
    assert(event != NULL);

    event->_type = UINT_MAX;
    event->_id = UINT_MAX;
    event->_data = NULL;
    event->_source = NULL;
}

void __ntg_listenable_init__(ntg_listenable* listenable, void* source)
{
    assert(listenable != NULL);
    assert(source != NULL);
    
    listenable->_source = source;
    listenable->_subs = ntg_event_sub_vec_new();
}

void __ntg_listenable_deinit__(ntg_listenable* listenable)
{
    assert(listenable != NULL);

    listenable->_source = NULL;
    ntg_event_sub_vec_destroy(listenable->_subs);
    listenable->_subs = NULL;
}

void ntg_listenable_raise(ntg_listenable* listenable, ntg_event* event)
{
    assert(listenable != NULL);

    ntg_event_sub_vec* subs = listenable->_subs;

    size_t i;
    for(i = 0; i < subs->_count; i++)
    {
        subs->_data[i].handler(subs->_data[i].subscriber, event);
    }
}

void ntg_listenable_listen(ntg_listenable* listenable,
        struct ntg_event_sub subscriber)
{
    assert(listenable != NULL);
    assert(subscriber.subscriber != NULL);
    assert(subscriber.handler != NULL);

    if(ntg_listenable_is_listening(listenable, subscriber.subscriber))
        return;

    ntg_event_sub_vec_append(listenable->_subs, subscriber);
}

void ntg_listenable_stop_listening(ntg_listenable* listenable, void* subscriber)
{
    assert(listenable != NULL);

    if(!ntg_listenable_is_listening(listenable, subscriber))
        return;

    ntg_event_sub_vec_remove_sub(listenable->_subs, subscriber);
}

bool ntg_listenable_is_listening(ntg_listenable* listenable, void* subscriber)
{
    assert(listenable != NULL);

    ntg_event_sub_vec* subs = listenable->_subs;

    size_t i;
    for(i = 0; i < subs->_count; i++)
    {
        if(subs->_data[i].subscriber == subscriber)
            return true;
    }

    return false;
}
