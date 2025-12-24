#ifndef _NTG_H_
#define _NTG_H_

#include "nt_event.h"
#include "nt_gfx.h"

#include "core/loop/ntg_loop.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/object/ntg_object.h"
#include "core/scene/ntg_def_scene.h"
#include "core/stage/ntg_def_stage.h"
#include "shared/ntg_shared.h"

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

void _ntg_init_();
void _ntg_deinit_();

/* -------------------------------------------------------------------------- */
/* LAUNCH */
/* -------------------------------------------------------------------------- */

typedef void (*ntg_gui_fn)(ntg_entity_system* system, void* data);

void ntg_launch(ntg_gui_fn gui_fn, void* data);

/* Waits for NTG thread to finish.
 * This function returns the return value of `gui_fn` passed in ntg_launch(). */
void* ntg_wait();

/* -------------------------------------------------------------------------- */
/* TO-DO LIST */
/* -------------------------------------------------------------------------- */

// TODO: ntg_padding: rethink measure_fn | *
// TODO: ntg_object/ntg_scene: rework scan_scene() fn, more event types, remove process_key_fn, add ntg_entity_feed_event()? | *
// TODO: nuterm: add signal event, maybe more event types - allow user to define and post events to loop? | **
// TODO: ntg_taskmaster: full rework | **
// TODO: ntg_list and/or ntg_table: implement | **
// TODO: ntg_scene: implement multi-root, multifocused? system | **
// TODO: ntg_scene: optimize ntg_def_scene layout fn | **
// TODO: nuterm-gfx: add convenience macros | *
// TODO: nuterm-gfx: add proper error-handling, names of nullable parameters in fns | ***

#endif // _NTG_H_
