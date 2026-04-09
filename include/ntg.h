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
#include "shared/ntg_log.h"
#include "base/ntg_type.h"
#include "base/ntg_cell.h"
#include "base/ntg_xy.h"
#include "base/ntg_sap.h"
#include "base/ntg_event.h"
#include "base/ntg_vecgrid.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/renderer/ntg_renderer.h"
#include "core/loop/ntg_loop.h"
#include "core/object/ntg_label.h"
#include "core/object/ntg_object_layout.h"
#include "core/object/ntg_object.h"
#include "core/object/ntg_border_style.h"
#include "core/object/ntg_prog_bar.h"
#include "core/object/ntg_color_block.h"
#include "core/object/ntg_main_panel.h"
#include "core/object/ntg_box.h"
#include "core/object/ntg_object_drawing.h"
#include "core/scene/ntg_scene.h"
#include "core/scene/ntg_focus_manager.h"
#include "core/scene/ntg_focus_scope.h"
#include "core/object/ntg_anchor_policy.h"
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

// TESTING:
// TODO: test ntg_scene_hit_test() and focus manager, scopes. Fix bug in ntg_object_hit_test()

// FEATURES:
// TODO: implement more widgets(table, list, button, text field...) | **

// RELEASE:
// TODO: raise more generic events
// TODO: add proper error-handling
// TODO: add proper documentation
// TODO: make .h files more readable
// TODO: hide functions from final .so file(change -fvisibility)
// BUG?: Horizontal resize causes line to appear at the bottom for split second

#endif // NTG_H
