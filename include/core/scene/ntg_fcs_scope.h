#ifndef NTG_FCS_SCOPE_H
#define NTG_FCS_SCOPE_H

#include "shared/ntg_shared.h"
#include "nt_event.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* OPTS */
/* ------------------------------------------------------ */

/* Is a click outside the focus scope ignored? */
enum ntg_fcs_scope_input_mode
{
    NTG_FCS_SCOPE_INPUT_MODELESS = 0,
    NTG_FCS_SCOPE_INPUT_MODAL
};

/* Does a click outside the focus scope clear focus? */
enum ntg_fcs_scope_out_click_mode
{
    NTG_FCS_SCOPE_OUT_CLICK_CLR = 0,
    NTG_FCS_SCOPE_OUT_CLICK_KEEP
};

/* Is pushing new focus scopes to fm stack allowed? */
enum ntg_fcs_scope_block_mode
{
    NTG_FCS_SCOPE_BLOCK_FALSE = 0,
    NTG_FCS_SCOPE_BLOCK_TRUE
};

/* What can each mouse button do */
enum ntg_fcs_scope_mouse_flag
{
    NTG_FCS_SCOPE_MOUSE_CAN_UNFCS = (1u << 1),
    NTG_FCS_SCOPE_MOUSE_CAN_FCS = (1u << 2)
};

/* Simulate mouse clicks by pressing keys */
struct ntg_fcs_scope_keys
{
    /* NT_KEY_ZERO = unbound */
    nt_key left_click, right_click, middle_click,
           scroll_up, scroll_down, cancel;
};

/* No keybinds */
static const struct ntg_fcs_scope_keys NTG_FCS_SCOPE_KEYS_ZERO = {0};

GENC_OPT_INLINE_DEF(ntg_fcs_scope_keys_opt, struct ntg_fcs_scope_keys)

/* ------------------------------------------------------ */
/* FCS SCOPE */
/* ------------------------------------------------------ */

/* Used in key & mouse handler function ctx */
struct ntg_fcs_scope_ctx
{
    ntg_widget* root;
    ntg_widget* last_focused;
    ntg_fcs_manager* fm;
    void* data;
};

struct ntg_fcs_scope
{
    /* Optional. */
    ntg_widget* root;

    enum ntg_fcs_scope_input_mode input_mode;
    enum ntg_fcs_scope_out_click_mode out_click_mode;
    enum ntg_fcs_scope_block_mode block_mode;

    /* Optional. Default binds `cancel` to ESC and `left_click` to ENTER. */
    ntg_fcs_scope_keys_opt keys;

    /* Optional. Default allows left and right click to focus/unfocus
     * and middle click to unfocus. */
    ntg_uint8_opt mouse_flags[5];

    /* Optional. Default is ntg_fcs_scope_dispatch_key_fn.
     * (if NULL caller will call default) */
    bool (*dispatch_key_fn)(const struct ntg_fcs_scope_ctx* ctx, nt_key key);

    /* Optional. Default is ntg_fcs_scope_dispatch_mouse_fn.
     * (if NULL caller will call default) */
    /* Mouse coordinates are mapped to `clicked` space. */
    bool (*dispatch_mouse_fn)(
            const struct ntg_fcs_scope_ctx* ctx,
            nt_mouse mouse,
            ntg_widget* clicked);

    void* data;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* KEY */
/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_scope_key_ignore_fn(const struct ntg_fcs_scope_ctx* ctx, nt_key key);

NTG_API bool
ntg_fcs_scope_key_dispatch_fn(const struct ntg_fcs_scope_ctx* ctx, nt_key key);

NTG_API bool
ntg_fcs_scope_key_bubble_fn(const struct ntg_fcs_scope_ctx* ctx, nt_key key);

/* ------------------------------------------------------ */
/* MOUSE */
/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_scope_mouse_ignore_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_widget* clicked);

NTG_API bool
ntg_fcs_scope_mouse_dispatch_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_widget* clicked);

NTG_API bool
ntg_fcs_scope_mouse_bubble_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_widget* clicked);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

bool ntg__fcs_scope_handle_keybind(
        const struct ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key);

void ntg__fcs_scope_handle_mouse_focus(
        const struct ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_widget* clicked);

#endif // NTG_FCS_SCOPE_H
