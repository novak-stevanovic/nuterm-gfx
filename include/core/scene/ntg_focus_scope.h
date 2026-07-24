#ifndef NTG_FOCUS_SCOPE_H
#define NTG_FOCUS_SCOPE_H

#include "shared/ntg_shared.h"
#include "nt_event.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

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

enum ntg_focus_scope_block_mode
{
    NTG_FOCUS_SCOPE_BLOCK_FALSE,
    NTG_FOCUS_SCOPE_BLOCK_TRUE
};

struct ntg_focus_scope_opts
{
    ntg_focus_scope_input_mode input_mode;
    ntg_focus_scope_out_click_mode out_click_mode;
    ntg_focus_scope_block_mode block_mode;
};

struct ntg_focus_scope_keybinds
{
    struct nt_key_event left_click_key,
            right_click_key, middle_click_key,
            scroll_up_key, scroll_down_key,
            cancel_key;
};

struct ntg_focus_scope_vtable
{
    bool (*handle_key_fn)(ntg_focus_scope* scope, struct nt_key_event key);

    // mouse coordinates are provided in `clicked` object space
    bool (*handle_mouse_fn)(
            ntg_focus_scope* scope,
            struct nt_mouse_event mouse,
            ntg_object* clicked);
};

struct ntg_focus_scope
{
    const struct ntg_focus_scope_vtable* __vtable;

    ntg_object* _root;
    ntg_focus_manager* _fm;

    struct ntg_focus_scope_keybinds _keybinds;
    struct ntg_focus_scope_opts _opts;

    ntg_object* _last_focused;
    bool __valid;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API struct ntg_focus_scope_opts
ntg_focus_scope_opts_def();

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_focus_scope_init(
        ntg_focus_scope* scope,
        ntg_object* scope_root,
        const struct ntg_focus_scope_keybinds* keybinds,
        const struct ntg_focus_scope_opts* opts,
        int* out_status);

NTG_API void
ntg_focus_scope_deinit(ntg_focus_scope* scope);

NTG_API void
ntg_focus_scope_init_move(
        ntg_focus_scope* dest,
        const ntg_focus_scope* src,
        int* out_status);

/* ------------------------------------------------------ */
/* SETTERS */
/* ------------------------------------------------------ */

NTG_API void
ntg_focus_scope_set_opts(
        ntg_focus_scope* scope,
        const struct ntg_focus_scope_opts* opts);

NTG_API void
ntg_focus_scope_set_keybinds(
        ntg_focus_scope* scope,
        const struct ntg_focus_scope_keybinds* keybinds);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_focus_scope_feed_key(ntg_focus_scope* scope, struct nt_key_event key);

NTG_API bool
ntg_focus_scope_feed_mouse(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_focus_scope_init_override(
        ntg_focus_scope* scope,
        const struct ntg_focus_scope_vtable* vtable,
        ntg_object* scope_root,
        const struct ntg_focus_scope_keybinds* keybinds,
        const struct ntg_focus_scope_opts* opts,
        int* out_status);

NTG_API bool
ntg_focus_scope_handle_key_fn(ntg_focus_scope* scope, struct nt_key_event key);

NTG_API bool
ntg_focus_scope_handle_key_bubble_fn(
        ntg_focus_scope* scope,
        struct nt_key_event key);

NTG_API bool
ntg_focus_scope_handle_mouse_fn(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked);

NTG_API bool
ntg_focus_scope_handle_mouse_bubble_fn(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked);

NTG_API extern const struct ntg_focus_scope_vtable
NTG_FOCUS_SCOPE_VTABLE_DEFAULT;

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_focus_scope_attach(ntg_focus_scope* scope, ntg_focus_manager* fm);

void _ntg_focus_scope_set_last_fcoused(ntg_focus_scope* scope, ntg_object* object);

void _ntg_focus_scope_invalidate(ntg_focus_scope* scope);

#endif // NTG_FOCUS_SCOPE_H
