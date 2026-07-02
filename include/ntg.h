#ifndef NTG_H
#define NTG_H

#include "nt_gfx.h"
#include "nt_event.h"

#include "thirdparty/uconv.h"
#include "thirdparty/sarena.h"
#include "thirdparty/genc.h"

#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"
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
#include "core/object/ntg_object_decorator.h"
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

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

typedef void (*ntg_gui_fn)(void* data);

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

/* Initializes logging to `ntg_log.txt` and initializes the terminal backend. Backend */
/* initialization failure triggers an assertion. */
void ntg_init();
/* Restores terminal cursor, alternate-screen, and mouse modes, then deinitializes the terminal */
/* library and logging. */
void ntg_deinit();

/* -------------------------------------------------------------------------- */
/* LAUNCH */
/* -------------------------------------------------------------------------- */


/* Enables terminal UI modes and starts `gui_fn(data)` on the library GUI thread. Only one launch */
/* is allowed at a time; a `NULL` callback is ignored, while allocation/thread failures assert. */
void ntg_launch(ntg_gui_fn gui_fn, void* data);

/* Joins the launched GUI thread. It returns immediately before any launch, but does not reset */
/* launch state; repeated waits after one launch attempt to join the same stored thread again. */
void ntg_wait();


#endif // NTG_H
