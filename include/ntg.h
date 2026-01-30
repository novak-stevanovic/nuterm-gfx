#ifndef NTG_H
#define NTG_H

#include "nt_gfx.h"
#include "nt_event.h"

#include "thirdparty/uconv.h"
#include "thirdparty/sarena.h"
#include "thirdparty/genc.h"

#include "shared/ntg_shared.h"
#include "shared/ntg_status.h"
#include "shared/ntg_convenience.h"
#include "shared/ntg_string.h"
#include "shared/ntg_log.h"
#include "base/ntg_type.h"
#include "base/ntg_cell.h"
#include "base/ntg_cleanup.h"
#include "base/ntg_xy.h"
#include "base/ntg_sap.h"
#include "base/ntg_event.h"
#include "base/ntg_vecgrid.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/renderer/ntg_renderer.h"
#include "core/loop/ntg_loop.h"
#include "core/object/ntg_label.h"
#include "core/object/ntg_object_layout.h"
#include "core/object/ntg_def_border.h"
#include "core/object/ntg_object.h"
#include "core/object/ntg_prog_bar.h"
#include "core/object/ntg_color_block.h"
#include "core/object/ntg_border_box.h"
#include "core/object/ntg_box.h"
#include "core/object/ntg_object_drawing.h"
#include "core/scene/ntg_scene.h"
#include "core/scene/ntg_scene_layer.h"
#include "core/stage/ntg_stage_drawing.h"
#include "core/stage/ntg_stage.h"

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

void ntg_init();
void ntg_deinit();

/* -------------------------------------------------------------------------- */
/* LAUNCH */
/* -------------------------------------------------------------------------- */

typedef void (*ntg_gui_fn)(void* data);

void ntg_launch(ntg_gui_fn gui_fn, void* data);

void ntg_wait();

/* -------------------------------------------------------------------------- */
/* TO-DO LIST */
/* -------------------------------------------------------------------------- */

// ACTIVE: ntg_widget refactor. | **

// TODO: implement modals on scene, object focus managers | **

// TODO: ntg_scene: implement multi-root system | **
// TODO: ntg_label: coloring/styling specific words | **
// TODO: nuterm-gfx: implement more widgets(table, list, group, button, text field...) | **

// RELEASE
// TODO: hide functions from final .so file(change -fvisibility)
// TODO: nuterm-gfx: add more generic events
// TODO: nuterm-gfx: add convenience macros
// TODO: nuterm-gfx: add proper error-handling
// TODO: nuterm-gfx: make comment documentation consistent

#endif // NTG_H
