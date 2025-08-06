#ifndef _NTG_EVENT_H_
#define _NTG_EVENT_H_

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

typedef struct ntg_listenable
{
    ntg_event_sub_vec* _subs;
} ntg_listenable;

typedef void (*ntg_event_handler)(void* subscriber, ntg_event* event);

struct ntg_event_sub
{
    void* subscriber;
    ntg_event_handler handler;
};

/* -------------------------------------------------------------------------- */

void __ntg_event_init__(ntg_event* event, uint type, void* source, void* data);
void __ntg_event_deinit__(ntg_event* event);

/* -------------------------------------------------------------------------- */

void __ntg_listenable_init__(ntg_listenable* listenable);
void __ntg_listenable_deinit__(ntg_listenable* listenable);

void ntg_listenable_raise(ntg_listenable* listenable, ntg_event* event);
void ntg_listenable_listen(ntg_listenable* listenable,
        struct ntg_event_sub subscription);
void ntg_listenable_stop_listening(ntg_listenable* listenable, void* subscriber);
bool ntg_listenable_is_listening(ntg_listenable* listenable, void* subscriber);

/* -------------------------------------------------------------------------- */

#endif // _NTG_EVENT_H_
