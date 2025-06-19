#ifndef _NTG_EVENT_H_
#define _NTG_EVENT_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#include "shared/ntg_status.h"

#define NTG_EVENT_TYPE_DEFAULT 0

typedef struct ntg_event ntg_event_t;

typedef void (*ntg_event_handler_t)(void* subscriber, ntg_event_t* event,
        void* data);

typedef uint ntg_event_type_t;

struct ntg_event_sub
{
    ntg_event_handler_t handler;
    void* subscriber;
};

ntg_event_t* ntg_event_create(ntg_event_type_t type, void* source);
void ntg_event_destroy(ntg_event_t* event);

void ntg_event_raise(ntg_event_t* event, void* data);

void ntg_event_subscribe(ntg_event_t* event, struct ntg_event_sub subscription,
        ntg_status_t* out_status);

void ntg_event_unsubscribe(ntg_event_t* event, const void* subscriber,
        ntg_status_t* out_status);

bool ntg_event_is_subscribed(const ntg_event_t* event, const void* subscriber);
ntg_event_type_t ntg_event_get_type(const ntg_event_t* event);
void* ntg_event_get_source(const ntg_event_t* event);

#endif // _NTG_EVENT_H_
