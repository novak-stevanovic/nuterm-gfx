#ifndef _NTG_H_
#define _NTG_H_

#include "nt_event.h"
#include "nt_gfx.h"

// TODO: implement ntg_box spacing
// TODO: change ntg_oxy
// TODO: object decorators: implement border & padding - MEDIUM
// TODO: ntg_scene: implement multi-root system - MEDIUM
// TODO: ntg_table: implement - MEDIUM
// TODO: ntg_list: implement - MEDIUM
// TODO: ntg_scene: implement ntg_fast_scene(performs full layout only when necessary) - MEDIUM
// TODO: ntg_taskmaster: what if a task gets stuck, what if deinit func is called before a task is finished?
// TODO: ntg_taskmaster: allow for abstract taskmaster?
// TODO: implement object fx interfaces for objects - EASY
// TODO: add proper error-handling - HARD

/* -------------------------------------------------------------------------- */

void __ntg_init__();
void __ntg_deinit__();

/* -------------------------------------------------------------------------- */

typedef void (*ntg_gui_fn)(void* data);

void ntg_launch(ntg_gui_fn gui_fn, void* data);

/* Waits for NTG thread to finish.
 * This function returns the return value of `gui_fn` passed in nui_launch(). */
void* ntg_wait();

/* -------------------------------------------------------------------------- */

#endif // _NTG_H_
