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
#include "shared/ntg_str.h"
#include "base/ntg_cell.h"
#include "base/ntg_xy.h"
#include "base/ntg_sap.h"
#include "base/ntg_event.h"
#include "base/ntg_vecgrid.h"
#include "base/ntg_type.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/renderer/ntg_renderer.h"
#include "core/loop/ntg_loop.h"
#include "core/object/ntg_object_layout.h"
#include "core/object/ntg_object.h"
#include "core/object/ntg_object_decorator.h"
#include "core/object/ntg_border_style.h"
#include "core/object/ntg_prog_bar.h"
#include "core/object/ntg_text.h"
#include "core/object/ntg_label.h"
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
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

enum ntg_mouse_mode { NTG_MOUSE_ENABLE, NTG_MOUSE_DISABLE };
enum ntg_alt_screen_mode { NTG_ALT_SCREEN_ENABLE, NTG_ALT_SCREEN_DISABLE };
enum ntg_cursor_mode { NTG_CURSOR_HIDE, NTG_CURSOR_SHOW };
enum ntg_unsupported_term_mode { NTG_UNSUPPORTED_TERM_IGNORE, NTG_UNSUPPORTED_TERM_STOP };

struct ntg_opts
{
    enum ntg_mouse_mode mouse_mode;
    enum ntg_alt_screen_mode alt_screen_mode;
    enum ntg_cursor_mode cursor_mode;
    enum ntg_unsupported_term_mode unsupported_term_mode;
};

/* Returns the default options used when `ntg_enable()` receives `NULL` opts.
 *
 * RETURN VALUE:
 * Mouse input and the alternate screen are enabled, the cursor is hidden, and
 * unsupported terminals are ignored. */
NTG_API struct ntg_opts
ntg_opts_def(void);

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes the underlying terminal library and applies the requested
 * terminal modes. Passing `NULL` for `opts` uses `ntg_opts_def()`. Passing
 * `NULL` for `log_filepath` disables file logging.
 *
 * ERROR CODES:
 * - `NTG_ERR_LOG_INIT_FAIL`: the log file cannot be opened.
 * - `NTG_ERR_UNSUPP_TERM`: the terminal is unsupported and the selected
 *   unsupported-terminal mode is `NTG_UNSUPPORTED_TERM_STOP`.
 * - `NTG_ERR_UNEXPECTED`: terminal initialization fails unexpectedly. */
NTG_API void
ntg_enable(
        const struct ntg_opts* opts,
        const char* log_filepath,
        int* out_status);

/* ------------------------------------------------------ */

/* Restores terminal modes changed by `ntg_enable()`, shuts down the underlying
 * terminal library, and closes the active log. Calling it before
 * `ntg_enable()` has no effect. */
NTG_API void
ntg_disable(void);

/* TODO:
 * add vtable to other classes: renderer, stage, scene, focus_scope, etc also add deinit_fn to allow inheritance
 * make handlers for on_key and on_mouse inside vtable, not inside hooks struct(make them return void)
 * adjust mouse pos when focus manager dispatches to focus scope and focus scope dispatches to object
 * remove comments from everywhere because they may be outdated. When adding them next time: inside return values never explain NTG_ERR_UNEXPECTED
*/

#endif // NTG_H
