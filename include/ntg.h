#ifndef _NTG_H_
#define _NTG_H_

#include "nt_event.h"
#include "nt_gfx.h"

// TODO: remove ntg_drawable?
// TODO: refactor draw fns for ntg_objects - MEDIUM, ACTIVE
// TODO: object decorators: implement border & padding - MEDIUM, ACTIVE
// TODO: nuterm: add signal event - EASY
// TODO: add convenience macros - EASY
// TODO: ntg_taskmaster: what if a task gets stuck, what if deinit func is called before a task is finished? - EASY
// TODO: ntg_object: rethink how to store children with both ntg_object and ntg_drawable references - EASY
// TODO: ntg_scene: implement multi-root system - MEDIUM
// TODO: ntg_list and/or ntg_table: implement - MEDIUM
// TODO: ntg_scene: optimize ntg_scene(performs full layout only when necessary) - MEDIUM
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
