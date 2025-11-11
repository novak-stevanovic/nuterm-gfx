#ifndef _NTG_EVENT_H_
#define _NTG_EVENT_H_

#include <stdbool.h>
#include <sys/types.h>

#define NTG_EVENT_TYPE_INVALID UINT_MAX 
#define NTG_EVENT_ID_INVALID UINT_MAX;

typedef struct ntg_event ntg_event;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef void (*ntg_event_handler)(void* subscriber, const ntg_event* event);

/* -------------------------------------------------------------------------- */

struct ntg_event
{
    uint _id;
    uint _type;
    void* _source;
    void* _data;
};

void __ntg_event_init__(ntg_event* event, uint type, void* source, void* data);

/* ------------------------------------------------------ */

ntg_event_delegate* ntg_event_delegate_new();
void ntg_event_delegate_destroy(ntg_event_delegate* del);

ntg_listenable* ntg_event_delegate_listenable(ntg_event_delegate* del);
void ntg_event_delegate_raise(ntg_event_delegate* del, ntg_event* event);

/* ------------------------------------------------------ */

void ntg_listenable_listen(ntg_listenable* listenable,
        void* subscriber, ntg_event_handler handler);
void ntg_listenable_stop_listening(ntg_listenable* listenable,
        void* subscriber, ntg_event_handler handler);

bool ntg_listenable_is_listening(ntg_listenable* listenable,
        void* subscriber);
bool ntg_listenable_has_sub(ntg_listenable* listenable,
        void* subscriber, ntg_event_handler handler);

/* -------------------------------------------------------------------------- */

#endif // _NTG_EVENT_H_
