#ifndef _NTG_LISTENABLE_H_
#define _NTG_LISTENABLE_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct ntg_event ntg_event;
typedef struct ntg_event_sub ntg_event_sub;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_del ntg_event_del;

/* -------------------------------------------------------------------------- */

void ntg_listenable_listen(ntg_listenable* listenable, struct ntg_event_sub sub);
void ntg_listenable_stop_listening(ntg_listenable* listenable, void* subscriber);
bool ntg_listenable_is_listening(ntg_listenable* listenable, void* subscriber);

/* -------------------------------------------------------------------------- */

ntg_event_del* ntg_event_del_new();
void ntg_event_del_destroy(ntg_event_del* del);

ntg_listenable* ntg_event_del_get_listenable(ntg_event_del* del);
void ntg_event_del_raise(ntg_event_del* del, ntg_event* event);

/* -------------------------------------------------------------------------- */

#endif // _NTG_LISTENABLE_H_
