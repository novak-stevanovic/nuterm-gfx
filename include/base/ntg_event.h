// CURRENTLY UNUSED

#ifndef NTG_EVENT_H
#define NTG_EVENT_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_event
{
    unsigned int type;
    void* source;
    void* data;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Allocates and initializes an empty event delegate.
 *
 * RETURN VALUE:
 * A new delegate, or `NULL` if allocation or initialization fails. */
NTG_API ntg_event_delegate*
ntg_event_delegate_new();
/* Detaches every existing binding from the delegate and releases the delegate.
 * Binding objects remain allocated and can still be passed to
 * `ntg_event_unbind`; passing `NULL` has no effect. */
NTG_API void
ntg_event_delegate_destroy(ntg_event_delegate* delegate);

/* Registers `handler_fn` and `subscriber` with a delegate and returns a binding
 * that can later be removed.
 *
 * RETURN VALUE:
 * The new binding, or `NULL` on error.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `delegate` or `handler_fn` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: the binding cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: the delegate cannot store the new binding. */
NTG_API ntg_event_binding*
ntg_event_bind(
        ntg_event_delegate* delegate,
        void* subscriber,
        void (*handler_fn)(void* subscriber, struct ntg_event event),
        int* out_status);

/* Invokes every handler currently bound to the delegate, passing the supplied
 * event. A `NULL` delegate is ignored. */
NTG_API void
ntg_event_raise(ntg_event_delegate* delegate, struct ntg_event event);
/* Removes and releases one event binding. Passing `NULL` or an already detached
 * binding has no effect. */
NTG_API void
ntg_event_unbind(ntg_event_binding* binding);

#endif // NTG_EVENT_H
