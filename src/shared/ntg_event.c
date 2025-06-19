#include "shared/ntg_event.h"
#include "shared/ntg_event_sub_vec.h"
#include <assert.h>
#include <stdlib.h>

struct ntg_event
{
    ntg_event_type_t type;
    void* source;
    ntg_event_sub_vec_t subs;
};

ntg_event_t* ntg_event_create(ntg_event_type_t type, void* source)
{
    ntg_event_t* new = (ntg_event_t*)malloc(sizeof(ntg_event_t));
    if(new == NULL) return NULL;

    new->type = type;
    new->source = source;
    __ntg_event_sub_vec_init__(&new->subs);

    return new;
}

void ntg_event_destroy(ntg_event_t* event)
{
    if(event == NULL) return;

    event->type = 0;
    event->source = NULL;
    __ntg_event_sub_vec_deinit__(&event->subs);

    free(event);
}

void ntg_event_raise(ntg_event_t* event, void* data)
{
    if(event == NULL) return;

    size_t i;
    for(i = 0; i < event->subs._count; i++)
    {
        event->subs._data[i].handler(event->subs._data[i].subscriber,
                event, data);
    }
}

void ntg_event_subscribe(ntg_event_t* event, struct ntg_event_sub subscription,
        ntg_status_t* out_status)
{
    ntg_event_sub_vec_append(&event->subs, subscription);
}

void ntg_event_unsubscribe(ntg_event_t* event, const void* subscriber,
        ntg_status_t* out_status)
{
    ntg_event_sub_vec_remove_sub(&event->subs, subscriber);
}

bool ntg_event_is_subscribed(const ntg_event_t* event, const void* subscriber)
{
    if(subscriber == NULL) return false;

    return (ntg_event_sub_vec_find_sub(&event->subs, subscriber) != -1);
}

ntg_event_type_t ntg_event_get_type(const ntg_event_t* event)
{
    return (event != NULL) ? event->type : 0;
}

void* ntg_event_get_source(const ntg_event_t* event)
{
    return (event != NULL) ? event->source : 0;
}
