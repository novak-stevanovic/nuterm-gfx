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
    NTG_FCS_SCOPE_INPUT_MODELESS,
    NTG_FCS_SCOPE_INPUT_MODAL
};

enum ntg_fcs_scope_out_click_mode
{
    NTG_FCS_SCOPE_OUT_CLICK_CLR,
    NTG_FCS_SCOPE_OUT_CLICK_KEEP
};

enum ntg_fcs_scope_block_mode
{
    NTG_FCS_SCOPE_BLOCK_FALSE,
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

NTG_API struct ntg_fcs_scope_opts
ntg_fcs_scope_opts_default(void);

/* ------------------------------------------------------ */
/* KEYBINDS */
/* ------------------------------------------------------ */

struct ntg_fcs_scope_key
{
    bool bound; // if (bound == false), `key` is ignored
    struct nt_key key;
};

// Helper constructor
NTG_API struct ntg_fcs_scope_key
ntg_fcs_scope_key_new(struct nt_key key);

struct ntg_fcs_scope_keys
{
    struct ntg_fcs_scope_key left_click,
            right_click, middle_click,
            scroll_up, scroll_down,
            cancel;
};

// Left click is bound to enter, cancel to escape
NTG_API extern const struct ntg_fcs_scope_keys
NTG_FCS_SCOPE_KEYS_DEFAULT;

/* ------------------------------------------------------ */
/* FOCUS SCOPE */
/* ------------------------------------------------------ */

struct ntg_fcs_scope
{
    const struct ntg_fcs_scope_vtable* __vtable;

    ntg_object* _root;
    ntg_fcs_manager* _fm;

    struct ntg_fcs_scope_keys _keys;
    struct ntg_fcs_scope_opts _opts;

    ntg_object* _last_focused;
    bool __valid;

    void* data;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_fcs_scope_init(
        ntg_fcs_scope* scope,
        ntg_object* scope_root,
        const struct ntg_fcs_scope_keys* keys,
        const struct ntg_fcs_scope_opts* opts);

NTG_API int
ntg_fcs_scope_deinit(ntg_fcs_scope* scope);

NTG_API int
ntg_fcs_scope_init_move(
        ntg_fcs_scope* dest,
        const ntg_fcs_scope* src);

/* ------------------------------------------------------ */
/* SETTERS */
/* ------------------------------------------------------ */

NTG_API int
ntg_fcs_scope_set_opts(
        ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_opts* opts);

NTG_API int
ntg_fcs_scope_set_keys(
        ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_keys* keys);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_scope_feed_key(ntg_fcs_scope* scope, struct nt_key key);

NTG_API bool
ntg_fcs_scope_feed_mouse(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_fcs_scope_vtable
{
    bool (*dispatch_key_fn)(ntg_fcs_scope* scope, struct nt_key key);

    // mouse coordinates are provided in `clicked` object space
    bool (*dispatch_mouse_fn)(
            ntg_fcs_scope* scope,
            struct nt_mouse mouse,
            ntg_object* clicked);
};


/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_fcs_scope_init_override(
        ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_vtable* vtable,
        ntg_object* scope_root,
        const struct ntg_fcs_scope_keys* keys,
        const struct ntg_fcs_scope_opts* opts);

NTG_API bool
ntg_fcs_scope_dispatch_key_fn(ntg_fcs_scope* scope, struct nt_key key);

NTG_API bool
ntg_fcs_scope_dispatch_key_bubble_fn(
        ntg_fcs_scope* scope,
        struct nt_key key);

NTG_API bool
ntg_fcs_scope_dispatch_mouse_fn(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked);

NTG_API bool
ntg_fcs_scope_dispatch_mouse_bubble_fn(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked);

NTG_API extern const struct ntg_fcs_scope_vtable
NTG_FCS_SCOPE_VTABLE_DEFAULT;

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

void _ntg_fcs_scope_attach(ntg_fcs_scope* scope, ntg_fcs_manager* fm);

void _ntg_fcs_scope_set_last_focused(ntg_fcs_scope* scope, ntg_object* object);

void _ntg_fcs_scope_invalidate(ntg_fcs_scope* scope);

#endif // NTG_FCS_SCOPE_H
