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

enum ntg_fcs_scope_input_mode
{
    NTG_FCS_SCOPE_INPUT_MODELESS = 0,
    NTG_FCS_SCOPE_INPUT_MODAL
};

enum ntg_fcs_scope_out_click_mode
{
    NTG_FCS_SCOPE_OUT_CLICK_CLR = 0,
    NTG_FCS_SCOPE_OUT_CLICK_KEEP
};

enum ntg_fcs_scope_block_mode
{
    NTG_FCS_SCOPE_BLOCK_FALSE = 0,
    NTG_FCS_SCOPE_BLOCK_TRUE
};

enum ntg_fcs_scope_mouse_flag
{
    NTG_FCS_SCOPE_MOUSE_CAN_UNFCS = (1u << 1),
    NTG_FCS_SCOPE_MOUSE_CAN_FCS = (1u << 2)
};

struct ntg_fcs_scope_opts
{
    enum ntg_fcs_scope_input_mode input_mode;
    enum ntg_fcs_scope_out_click_mode out_click_mode;
    enum ntg_fcs_scope_block_mode block_mode;

    /* Index by using nt_mouse_event_type enum */
    uint8_t mouse_flags[5];
};

/* ------------------------------------------------------ */
/* KEYBINDS */
/* ------------------------------------------------------ */

struct ntg_fcs_scope_keys
{
    nt_key left_click, right_click, middle_click,
           scroll_up, scroll_down, cancel;
};

/* ------------------------------------------------------ */
/* HANDLERS */
/* ------------------------------------------------------ */

struct ntg_fcs_scope_ctx
{
    ntg_object* root;
    ntg_object* last_focused;
    ntg_fcs_manager* fm;
    void* data;
};

struct ntg_fcs_scope_handlers
{
    bool (*dispatch_key_fn)(const struct ntg_fcs_scope_ctx* ctx, nt_key key);

    // mouse coordinates are provided in `clicked` object space
    bool (*dispatch_mouse_fn)(
            const struct ntg_fcs_scope_ctx* ctx,
            nt_mouse mouse,
            ntg_object* clicked);
};

struct ntg_fcs_scope
{
    ntg_object* root;
    struct ntg_fcs_scope_keys keys;
    struct ntg_fcs_scope_opts opts;
    struct ntg_fcs_scope_handlers handlers;
    void* data;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ========================================================================== */
/* HANDLERS */
/* ========================================================================== */

NTG_API bool
ntg_fcs_scope_dispatch_key_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key);

NTG_API bool
ntg_fcs_scope_dispatch_key_bubble_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key);

NTG_API bool
ntg_fcs_scope_dispatch_mouse_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_object* clicked);

NTG_API bool
ntg_fcs_scope_dispatch_mouse_bubble_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_object* clicked);

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
        ntg_object* clicked);

#endif // NTG_FCS_SCOPE_H
