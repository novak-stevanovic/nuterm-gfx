#ifndef _NTG_EVENT_H_
#define _NTG_EVENT_H_

#include <stdbool.h>
#include <sys/types.h>

#define NTG_EVENT_TYPE_INVALID UINT_MAX 
#define NTG_EVENT_ID_INVALID UINT_MAX;

typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_dlgt ntg_event_dlgt;

/* -------------------------------------------------------------------------- */

struct ntg_event
{
    unsigned int id;
    unsigned int type;
    void* source;
    void* data;
};

typedef void (*ntg_event_handler)(void* subscriber, struct ntg_event event);

/* ------------------------------------------------------ */

ntg_event_dlgt* ntg_event_dlgt_new();
void ntg_event_dlgt_destroy(ntg_event_dlgt* event_dlgt);

ntg_listenable* ntg_event_dlgt_listenable(ntg_event_dlgt* event_dlgt);
void ntg_event_dlgt_raise(ntg_event_dlgt* event_dlgt, unsigned int type,
        void* source, void* data);

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
