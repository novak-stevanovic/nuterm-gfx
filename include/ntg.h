#ifndef _NTG_H_
#define _NTG_H_

#include "nt_event.h"
#include "nt_gfx.h"

// TODO: test ntg scene process key order
// TODO: implement border & padding as decorators
// TODO: ntg_cdrawable_vec_view instead of const ntg_drawable_vec* (???)
// TODO: implement allocator wrapper
// TODO: implement ntg_def_platform
// TODO: implement scene multi-root system
// TODO: implement object fx interfaces for objects

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
