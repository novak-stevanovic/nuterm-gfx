#ifndef _NTG_EVENT_H_
#define _NTG_EVENT_H_

#include <stdbool.h>
#include <stddef.h>

#include "base/ntg_event_sub_vec.h"

typedef struct ntg_event
{
    uint _id;
    uint _type;
    void* _source;
    void* _data;
} ntg_event;

typedef struct ntg_event_sub_delegate
{
    ntg_event_sub_vec __subs;
} ntg_event_sub_delegate;

typedef struct ntg_event_delegate
{
    ntg_event_sub_delegate _sub_delegate;
} ntg_event_delegate;

typedef void (*ntg_event_handler_fn)(void* subscriber, const ntg_event* event);

/* Event subscriber */
struct ntg_event_sub
{
    ntg_event_handler_fn handler;
    void* subscriber;
};

void __ntg_event_init__(ntg_event* event, uint type, void* source, void* data);
void __ntg_event_deinit__(ntg_event* event);

/* -------------------------------------------------------------------------- */

void __ntg_event_sub_delegate_init__(ntg_event_sub_delegate* sub_delegate);
void __ntg_event_sub_delegate_deinit__(ntg_event_sub_delegate* sub_delegate);

void ntg_event_sub_delegate_sub(ntg_event_sub_delegate* sub_delegate,
        struct ntg_event_sub subscription);
void ntg_event_sub_delegate_unsub(ntg_event_sub_delegate* sub_delegate,
        void* subscriber);
bool ntg_event_sub_delegate_is_sub(ntg_event_sub_delegate* sub_delegate,
        void* subscriber);

/* -------------------------------------------------------------------------- */

void __ntg_event_delegate_init__(ntg_event_delegate* delegate);
void __ntg_event_delegate_deinit__(ntg_event_delegate* delegate);

void ntg_event_delegate_raise(ntg_event_delegate* delegate, ntg_event* event);

#endif // _NTG_EVENT_H_
