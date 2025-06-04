#include "ntg_shared/ntg_event.h"
#include <assert.h>
#include <stdlib.h>

#define MAX_SUBS 15

struct ntg_event
{
    ntg_event_type_t type;
    void* source;
    struct
    {
        ntg_event_handler_t handler;
        void* subscriber;
    } subs[MAX_SUBS];
    size_t sub_count;
};

ntg_event_t* ntg_event_create(ntg_event_type_t type, void* source)
{
    ntg_event_t* new = (ntg_event_t*)malloc(sizeof(ntg_event_t));
    if(new == NULL) return NULL;

    new->type = type;
    new->sub_count = 0;
    new->source = source;

    return new;
}

void ntg_event_destroy(ntg_event_t* event)
{
    if(event == NULL) return;

    event->type = 0;
    event->sub_count = 0;
    event->source = NULL;
    free(event);
}

void ntg_event_raise(ntg_event_t* event, void* data)
{
    if(event == NULL) return;

    size_t i;
    for(i = 0; i < event->sub_count; i++)
    {
        event->subs[i].handler(event->subs[i].subscriber, event, data);
    }
}

void ntg_event_subscribe(ntg_event_t* event, ntg_event_handler_t handler,
        void* subscriber, ntg_status_t* out_status)
{
    assert(event->sub_count < MAX_SUBS);

    event->subs[event->sub_count].handler = handler;
    event->subs[event->sub_count].subscriber = subscriber;
}

void ntg_event_unsubscribe(ntg_event_t* event, const void* subscriber,
        ntg_status_t* out_status)
{
    assert(0);
}

bool ntg_event_is_subscribed(const ntg_event_t* event, const void* subscriber)
{
    if(subscriber == NULL) return false;

    size_t i;
    for(i = 0; i < event->sub_count; i++)
    {
        if(event->subs[i].subscriber == subscriber)
            return true;
    }

    return false;
}

ntg_event_type_t ntg_event_get_type(const ntg_event_t* event)
{
    return (event != NULL) ? event->type : 0;
}

void* ntg_event_get_source(const ntg_event_t* event)
{
    return (event != NULL) ? event->source : 0;
}
