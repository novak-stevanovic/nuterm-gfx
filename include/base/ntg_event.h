

#ifndef NTG_EVENT_H
#define NTG_EVENT_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_event
{
    unsigned int type;
    void* source;
    void* data;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */


NTG_API ntg_event_delegate*
ntg_event_delegate_new(void);

NTG_API void
ntg_event_delegate_destroy(ntg_event_delegate* delegate);


NTG_API ntg_event_binding*
ntg_event_bind(
        ntg_event_delegate* delegate,
        void* subscriber,
        void (*handler_fn)(void* subscriber, struct ntg_event event),
        int* out_status);


NTG_API void
ntg_event_raise(ntg_event_delegate* delegate, struct ntg_event event);

NTG_API void
ntg_event_unbind(ntg_event_binding* binding);

#endif // NTG_EVENT_H
