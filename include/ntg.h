#ifndef _NTG_H_
#define _NTG_H_

#include "nt_gfx.h"
#include "nt_event.h"
#include "nt_key.h"
#include "sarena.h"
#include "uconv.h"

#include "shared/ntg_status.h"
#include "shared/ntg_string.h"
#include "shared/ntg_log.h"
#include "shared/ntg_typedef.h"
#include "base/ntg_cell.h"
#include "core/entity/ntg_entity.h"
#include "core/entity/ntg_entity_type.h"
#include "core/entity/ntg_event_type.h"
#include "base/ntg_sap.h"
#include "base/ntg_xy.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/renderer/ntg_renderer.h"
#include "core/loop/ntg_loop.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "core/object/shared/ntg_object_measure_map.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/ntg_label.h"
#include "core/object/ntg_def_border.h"
#include "core/object/ntg_object.h"
#include "core/object/ntg_prog_bar.h"
#include "core/object/ntg_def_padding.h"
#include "core/object/ntg_color_block.h"
#include "core/object/ntg_border_box.h"
#include "core/object/ntg_box.h"
#include "core/object/ntg_padding.h"
#include "core/scene/ntg_scene.h"
#include "core/scene/ntg_scene_graph.h"
#include "core/scene/ntg_def_scene.h"
#include "core/scene/focuser/ntg_def_focuser.h"
#include "core/scene/focuser/ntg_focuser.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "core/stage/ntg_stage.h"
#include "core/stage/ntg_def_stage.h"

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

void ntg_init();
void ntg_deinit();

/* -------------------------------------------------------------------------- */
/* LAUNCH */
/* -------------------------------------------------------------------------- */

typedef void (*ntg_gui_fn)(ntg_entity_system* system, ntg_loop* loop, void* data);

void ntg_launch(ntg_gui_fn gui_fn, void* data);

void ntg_wait();

/* -------------------------------------------------------------------------- */
/* TO-DO LIST */
/* -------------------------------------------------------------------------- */

// TODO: ntg_object: notify scene when padding/border changes | *
// TODO: nuterm-gfx: rethink vectors - use lists instead?
// TODO: nuterm, sarena, uconv, nuterm-gfx: change macro names - no underscores?

// TODO: ntg_scene: implement multi-root system | **
// TODO: ntg_taskmaster: full rework | **
// TODO: ntg_list and/or ntg_table: implement | **
// TODO: ntg_label: coloring/styling specific words, implement buttons/text fields | *

// RELEASE
// TODO: nuterm-gfx: add more generic events
// TODO: nuterm-gfx: add convenience macros
// TODO: nuterm-gfx: add proper error-handling
// TODO: nuterm-gfx: make comment documentation consistent

#endif // _NTG_H_
