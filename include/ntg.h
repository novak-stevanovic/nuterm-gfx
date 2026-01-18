#ifndef NTG_H
#define NTG_H

#include "nt_gfx.h"
#include "nt_event.h"
#include "shared/sarena.h"
#include "shared/uconv.h"

#include "shared/ntg_status.h"
#include "shared/ntg_typedef.h"
#include "shared/uconv.h"
#include "shared/ntg_xy.h"
#include "shared/ntg_string.h"
#include "shared/ntg_shared.h"
#include "shared/sarena.h"
#include "shared/genc.h"
#include "shared/ntg_vecgrid.h"
#include "shared/ntg_log.h"
#include "base/ntg_cell.h"
#include "base/ntg_sap.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/renderer/ntg_renderer.h"
#include "core/loop/ntg_loop.h"
#include "core/entity/ntg_entity.h"
#include "core/entity/ntg_entity_type.h"
#include "core/entity/ntg_event_type.h"
#include "core/object/widget/ntg_label.h"
#include "core/object/widget/ntg_prog_bar.h"
#include "core/object/widget/ntg_color_block.h"
#include "core/object/widget/ntg_border_box.h"
#include "core/object/widget/ntg_box.h"
#include "core/object/widget/ntg_widget.h"
#include "core/object/widget/shared/ntg_widget_map.h"
#include "core/object/widget/shared/ntg_widget_xy_map.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "core/object/shared/ntg_object_map.h"
#include "core/object/widget/shared/ntg_widget_size_map.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/ntg_object.h"
#include "core/object/decorator/ntg_def_border.h"
#include "core/object/decorator/ntg_def_padding.h"
#include "core/object/decorator/ntg_decorator.h"
#include "core/scene/ntg_scene.h"
#include "core/scene/ntg_def_scene.h"
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

// ACTIVE: ntg_widget refactor. | **
// REMINDER: deattach object on deinit
// REMINDER: implement children for border_box, box.
// REMINDER: implement padding/border fot ntg_widget

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
