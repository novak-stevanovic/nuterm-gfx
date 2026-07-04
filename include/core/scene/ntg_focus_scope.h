#ifndef NTG_FOCUS_SCOPE_H
#define NTG_FOCUS_SCOPE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

enum ntg_focus_scope_input_mode
{
    NTG_FOCUS_SCOPE_INPUT_MODELESS,
    NTG_FOCUS_SCOPE_INPUT_MODAL
};

enum ntg_focus_scope_out_click_mode
{
    NTG_FOCUS_SCOPE_OUT_CLICK_KEEP,
    NTG_FOCUS_SCOPE_OUT_CLICK_CLR
};

// Forbids pushing new scopes onto the stack
enum ntg_focus_scope_block_mode
{
    NTG_FOCUS_SCOPE_BLOCK_FALSE,
    NTG_FOCUS_SCOPE_BLOCK_TRUE
};

struct ntg_focus_key_ctx
{
    ntg_focus_manager* fm;
    ntg_object* scope_root;
};

struct ntg_focus_mouse_ctx
{
    ntg_focus_manager* fm;
    ntg_object* scope_root;

    ntg_object* clicked;
    struct ntg_xy adj_pos; // position inside scope root space
};

struct ntg_focus_scope
{
    ntg_object* root;

    ntg_focus_scope_input_mode input_mode;
    ntg_focus_scope_out_click_mode out_click_mode;
    ntg_focus_scope_block_mode block_mode;

    bool (*on_key_fn)(
            void* data,
            const struct ntg_focus_key_ctx* ctx,
            struct nt_key_event key);

    bool (*on_mouse_fn)(
            void* data,
            const struct ntg_focus_mouse_ctx* ctx,
            struct nt_mouse_event mouse);
    void* data;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

/* Default key handler that uses `ctx` to forward `key` to the focused object.
 * The `_` callback data argument is ignored.
 *
 * RETURN VALUE:
 * `true` when the focused object handles the event; otherwise `false`. */
NTG_API bool
ntg_focus_scope_dispatch_key(
        void* _,
        const struct ntg_focus_key_ctx* ctx,
        struct nt_key_event key);

/* -------------------------------------------------------------------------- */

/* Static mouse handler that uses `ctx` to forward `mouse` to the clicked object
 * without changing focus. The `_` callback data argument is ignored.
 *
 * RETURN VALUE:
 * `true` when the clicked object handles the event; otherwise `false`. */
NTG_API bool
ntg_focus_scope_dispatch_mouse_stc(
        void* _,
        const struct ntg_focus_mouse_ctx* ctx,
        struct nt_mouse_event mouse);

/* -------------------------------------------------------------------------- */

/* Dynamic mouse handler that uses `ctx` to update focus from the clicked object
 * and then dispatches `mouse`. Clicking elsewhere may clear old focus. The `_`
 * callback data argument is ignored.
 *
 * RETURN VALUE:
 * `true` when the click causes focus/dispatch handling; otherwise `false`. */
NTG_API bool
ntg_focus_scope_dispatch_mouse_dyn(
        void* _,
        const struct ntg_focus_mouse_ctx* ctx,
        struct nt_mouse_event mouse);

#endif // NTG_FOCUS_SCOPE_H
