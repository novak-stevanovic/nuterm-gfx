#ifndef _NTG_H_
#define _NTG_H_

#include "nt_event.h"
#include "nt_gfx.h"

// TODO: finish implementing task exec mechanism - EASY
// TODO: test ntg scene process key order - EASY
// TODO: ntg_cdrawable_vec_view instead of const ntg_drawable_vec* (???) - EASY
// TODO: add arena allocator to in-loop functions - EASY
// TODO: implement border & padding as decorators - MEDIUM
// TODO: implement scene multi-root system - MEDIUM
// TODO: implement ntg_table - MEDIUM
// TODO: implement ntg_list - MEDIUM
// TODO: implement ntg_fast_scene(performs full layout only when necessary) - MEDIUM
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
