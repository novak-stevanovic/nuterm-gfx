#ifndef _NTG_LISTENABLE_H_
#define _NTG_LISTENABLE_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct ntg_event ntg_event;
typedef struct ntg_event_sub ntg_event_sub;
typedef struct ntg_listenable ntg_listenable;

/* -------------------------------------------------------------------------- */

void __ntg_listenable_init__(ntg_listenable* listenable);
void __ntg_listenable_deinit__(ntg_listenable* listenable);

ntg_listenable* ntg_listenable_new();
void ntg_listenable_destroy(ntg_listenable* listenable);

void ntg_listenable_raise(ntg_listenable* listenable, ntg_event* event);
void ntg_listenable_listen(ntg_listenable* listenable, struct ntg_event_sub sub);
void ntg_listenable_stop_listening(ntg_listenable* listenable, void* subscriber);
bool ntg_listenable_is_listening(ntg_listenable* listenable, void* subscriber);

/* -------------------------------------------------------------------------- */

#endif // _NTG_LISTENABLE_H_
