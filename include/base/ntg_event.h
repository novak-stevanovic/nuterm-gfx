#ifndef NTG_EVENT_H
#define NTG_EVENT_H

#include "shared/ntg_shared.h"

struct ntg_event
{
    unsigned int type;
    void* source;
    void* data;
};

ntg_event_delegate* ntg_event_delegate_new();
void ntg_event_delegate_destroy(ntg_event_delegate* delegate);

ntg_event_binding* ntg_event_bind(
        ntg_event_delegate* delegate,
        void* subscriber,
        void (*handler_fn)(void* subscriber, struct ntg_event event));

void ntg_event_raise(ntg_event_delegate* delegate, struct ntg_event event);
void ntg_event_unbind(ntg_event_binding* binding);

#endif // NTG_EVENT_H
