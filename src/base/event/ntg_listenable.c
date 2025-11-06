#include <assert.h>
#include <stdlib.h>

#include "base/event/ntg_listenable.h"
#include "base/event/ntg_event_sub_vec.h"
#include "base/event/ntg_event.h"

struct ntg_listenable
{
    ntg_event_sub_vec* subs;
};

void __ntg_listenable_init__(ntg_listenable* listenable)
{
    assert(listenable != NULL);
    
    listenable->subs = ntg_event_sub_vec_new();
}

void __ntg_listenable_deinit__(ntg_listenable* listenable)
{
    assert(listenable != NULL);

    ntg_event_sub_vec_destroy(listenable->subs);
    listenable->subs = NULL;
}

ntg_listenable* ntg_listenable_new()
{
    ntg_listenable* new = (ntg_listenable*)malloc(sizeof(ntg_listenable));
    assert(new != NULL);

    __ntg_listenable_init__(new);

    return new;
}

void ntg_listenable_destroy(ntg_listenable* listenable)
{
    assert(listenable != NULL);

    __ntg_listenable_deinit__(listenable);
    free(listenable);
}

void ntg_listenable_raise(ntg_listenable* listenable, ntg_event* event)
{
    assert(listenable != NULL);

    ntg_event_sub_vec* subs = listenable->subs;

    size_t i;
    for(i = 0; i < subs->_count; i++)
    {
        subs->_data[i].handler(subs->_data[i].subscriber, event);
    }
}

void ntg_listenable_add_sub(ntg_listenable* listenable, struct ntg_event_sub sub)
{
    assert(listenable != NULL);
    assert(sub.subscriber != NULL);
    assert(sub.handler != NULL);

    if(ntg_listenable_is_listening(listenable, sub.subscriber))
        return;

    ntg_event_sub_vec_append(listenable->subs, sub);
}

void ntg_listenable_stop_listening(ntg_listenable* listenable, void* subscriber)
{
    assert(listenable != NULL);

    if(!ntg_listenable_is_listening(listenable, subscriber))
        return;

    ntg_event_sub_vec_remove_sub(listenable->subs, subscriber);
}

bool ntg_listenable_is_listening(ntg_listenable* listenable, void* subscriber)
{
    assert(listenable != NULL);

    ntg_event_sub_vec* subs = listenable->subs;

    size_t i;
    for(i = 0; i < subs->_count; i++)
    {
        if(subs->_data[i].subscriber == subscriber)
            return true;
    }

    return false;
}
