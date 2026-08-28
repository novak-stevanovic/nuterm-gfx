#ifndef NTG_H
#define NTG_H

#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"
#include "shared/ntg_convenience.h"
#include "shared/ntg_log.h"
#include "shared/ntg_str.h"
#include "base/ntg_cell.h"
#include "base/ntg_xy.h"
#include "base/ntg_sap.h"
#include "base/object/ntg_event.h"
#include "base/object/ntg_type.h"
#include "base/object/ntg_object.h"
#include "core/renderer/ntg_db_renderer.h"
#include "core/renderer/ntg_renderer.h"
#include "core/loop/ntg_loop.h"
#include "core/widget/ntg_widget_layout.h"
#include "core/widget/ntg_widget.h"
#include "core/widget/border/ntg_border_style.h"
#include "core/widget/border/ntg_border_9x.h"
#include "core/widget/ntg_prog_bar.h"
#include "core/widget/ntg_text.h"
#include "core/widget/ntg_label.h"
#include "core/widget/ntg_button.h"
#include "core/widget/ntg_clr_block.h"
#include "core/widget/ntg_main_panel.h"
#include "core/widget/ntg_box.h"
#include "core/widget/ntg_widget_vec.h"
#include "core/scene/ntg_scene.h"
#include "core/scene/ntg_fcs_manager.h"
#include "core/scene/ntg_fcs_scope.h"
#include "core/widget/anchor/ntg_anchor_policy.h"
#include "core/widget/anchor/ntg_float.h"
#include "core/widget/anchor/ntg_sidefloat.h"
#include "core/stage/ntg_stage.h"
#include "core/task/ntg_task_runner.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

enum ntg_alt_screen_mode
{ 
    NTG_ALT_SCREEN_ENABLE = 0,
    NTG_ALT_SCREEN_DISABLE
};

enum ntg_unsupported_term_mode 
{ 
    NTG_UNSUPPORTED_TERM_IGNORE = 0,
    NTG_UNSUPPORTED_TERM_STOP
};

struct ntg_opts
{
    enum ntg_alt_screen_mode alt_screen_mode;
    enum ntg_unsupported_term_mode unsupported_term_mode;
};

static const struct ntg_opts NTG_OPTS_ZERO = {0};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_enable(const struct ntg_opts* opts, const char* log_filepath);

NTG_API const struct ntg_opts*
ntg_get_opts(void);

NTG_API void
ntg_disable(void);

/* TODO LIST

FIX:
rename: main_panel -> ???
rename: NTG_MAIN_PANEL_NORTH -> ???
pos -> NTG_SIZE_MAX for no draw
ntg_button: disable() or ntg_widget: disable()?
add_text, rm_text optimization fns for ntg_text() for ntg_input

IMPLEMENT:
ntg_input
ntg_list
ntg_dialogue
ntg_popup

FINISH:
Error codes: change values
add some old useful comments that were deleted
add comments with explanations: When adding them next time: inside return values never explain NTG_ERR_UNEXPECTED, fix spacing from old removed comments
ntg_log()

TEST:
frame times

AFTER RELEASE:
optimize layout_layer(): if scene is dirty all layer trees are traversed, even clean ones

*/

#endif // NTG_H
