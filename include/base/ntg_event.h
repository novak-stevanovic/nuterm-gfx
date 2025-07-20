#ifndef _NTG_EVENT_H_
#define _NTG_EVENT_H_

#include "base/ntg_event_sub_vec.h"
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#define NTG_EVENT_TYPE_INVALID UINT_MAX 
#define NTG_EVENT_ID_INVALID UINT_MAX;

typedef struct ntg_event_sub_vec ntg_event_sub_vec;

typedef struct ntg_event
{
    uint _id;
    uint _type;
    void* _source;
    void* _data;
} ntg_event;

/* Keep this internal to prevent other objects from raising the event */
typedef struct ntg_event_delegate
{
    ntg_event_sub_vec _subs;
} ntg_event_delegate;

/* Expose this to allow other objects to subscribe/unsubscribe */
typedef struct ntg_event_delegate_view
{
    ntg_event_delegate* __delegate;
} ntg_event_delegate_view;

typedef void (*ntg_event_handler)(void* subscriber, ntg_event* event);

struct ntg_event_sub
{
    void* subscriber;
    ntg_event_handler handler;
};

/* -------------------------------------------------------------------------- */

void __ntg_event_init__(ntg_event* event, uint type, void* source, void* data);
void __ntg_event_deinit__(ntg_event* event);

/* ------------------------------------------------------ */

void __ntg_event_delegate_init__(ntg_event_delegate* delegate);
void __ntg_event_delegate_deinit__(ntg_event_delegate* delegate);

void ntg_event_delegate_raise(ntg_event_delegate* delegate, ntg_event* event);

/* ------------------------------------------------------ */

ntg_event_delegate_view ntg_event_delegate_view_new(ntg_event_delegate* delegate);

void ntg_event_delegate_view_sub(ntg_event_delegate_view* view,
        struct ntg_event_sub subscription);
void ntg_event_delegate_view_unsub(ntg_event_delegate_view* view,
        void* subscriber);
bool ntg_event_delegate_view_is_subbed(const ntg_event_delegate_view* view,
        void* subscriber);

/* -------------------------------------------------------------------------- */

#endif // _NTG_EVENT_H_
