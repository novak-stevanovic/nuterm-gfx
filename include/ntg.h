#ifndef _NTG_H_
#define _NTG_H_

#include "nt_event.h"
#include "nt_gfx.h"

// TODO: object decorators: implement border & padding - MEDIUM, ACTIVE
// TODO: ntg_taskmaster: what if a task gets stuck, what if deinit func is called before a task is finished? - EASY
// TODO: ntg_object: ntg_object_get_children() returns const ntg_object_vec* - EASY
// TODO: global: add ntg_allocator for faster object initialization? - EASY
// TODO: ntg_list and/or ntg_table: implement - MEDIUM
// TODO: ntg_scene: implement multi-root system - MEDIUM
// TODO: ntg_scene: optimize ntg_def_scene layout fn - MEDIUM
// TODO: nuterm: add signal event, maybe more event types? - EASY
// TODO: add convenience macros - EASY
// TODO: add proper error-handling - HARD

/* -------------------------------------------------------------------------- */

void _ntg_init_();
void _ntg_deinit_();

/* -------------------------------------------------------------------------- */

typedef void (*ntg_gui_fn)(void* data);

void ntg_launch(ntg_gui_fn gui_fn, void* data);

/* Waits for NTG thread to finish.
 * This function returns the return value of `gui_fn` passed in nui_launch(). */
void* ntg_wait();

/* -------------------------------------------------------------------------- */

#endif // _NTG_H_
