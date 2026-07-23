#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

struct ntg_focus_scope_opts ntg_focus_scope_opts_def()
{
    return (struct ntg_focus_scope_opts) {
        .input_mode = NTG_FOCUS_SCOPE_INPUT_MODELESS,
        .out_click_mode = NTG_FOCUS_SCOPE_OUT_CLICK_KEEP,
        .block_mode = NTG_FOCUS_SCOPE_BLOCK_FALSE
    };
};

static bool handle_key_keybind(ntg_focus_scope* scope, struct nt_key_event key)
{
    // TODO
    return false;
}

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_focus_scope_init(
        ntg_focus_scope* scope,
        ntg_object* scope_root,
        const struct ntg_focus_scope_keybinds* keybinds,
        const struct ntg_focus_scope_opts* opts,
        bool (*handle_key_fn)(ntg_focus_scope* scope, struct nt_key_event key),
        bool (*handle_mouse_fn)(ntg_focus_scope* scope, struct nt_mouse_event mouse),
        int* out_status)
{
    ntg_init_status(out_status);

    if(!scope)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    scope->_scope_root = scope_root;
    scope->_fm = NULL;

    scope->__handle_key_fn = handle_key_fn;
    scope->__handle_mouse_fn = handle_mouse_fn;

    ntg_focus_scope_set_keybinds(scope, keybinds);
    ntg_focus_scope_set_opts(scope, opts);
}

void ntg_focus_scope_deinit(ntg_focus_scope* scope)
{
    if(!scope) return;

    scope->_fm = NULL;
    scope->_scope_root = NULL;
    scope->_keybinds = (struct ntg_focus_scope_keybinds) {0};
    scope->_opts = ntg_focus_scope_opts_def();
    scope->__handle_key_fn = NULL;
    scope->__handle_mouse_fn = NULL;
}

void ntg_focus_scope_init_move(
        ntg_focus_scope* dest,
        const ntg_focus_scope* src,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!dest || !src)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    (*dest) = (*src);
}

/* ------------------------------------------------------ */
/* SETTERS */
/* ------------------------------------------------------ */

void ntg_focus_scope_set_opts(
        ntg_focus_scope* scope,
        const struct ntg_focus_scope_opts* opts)
{
    if(!scope) return;

    scope->_opts = (opts ? (*opts) : ntg_focus_scope_opts_def());
}

void ntg_focus_scope_set_keybinds(
        ntg_focus_scope* scope,
        const struct ntg_focus_scope_keybinds* keybinds)
{
    if(!scope) return;
    
    struct ntg_focus_scope_keybinds zero_keybinds = {0};
    scope->_keybinds = (keybinds ? (*keybinds) : zero_keybinds);
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_focus_scope_feed_key(ntg_focus_scope* scope, struct nt_key_event key)
{
    if(!scope) return false;

    if(handle_key_keybind(scope, key))
        return true;

    if(scope->__handle_key_fn)
        return scope->__handle_key_fn(scope, key);
    else
        return false;
}

bool ntg_focus_scope_feed_mouse(ntg_focus_scope* scope, struct nt_mouse_event mouse)
{
    if(!scope) return false;

    return scope->__handle_mouse_fn(scope, mouse);
}

/* ------------------------------------------------------ */
/* DEFAULT VIRTUAL FUNCTIONS */
/* ------------------------------------------------------ */

NTG_API bool
ntg_focus_scope_handle_key_fn(ntg_focus_scope* scope, struct nt_key_event key)
{
}

NTG_API bool
ntg_focus_scope_handle_key_bubble_fn(ntg_focus_scope* scope, struct nt_key_event key)
{
}

NTG_API bool
ntg_focus_scope_handle_mouse_fn(ntg_focus_scope* scope, struct nt_mouse_event mouse)
{
}

NTG_API bool
ntg_focus_scope_handle_mouse_bubble_fn(ntg_focus_scope* scope, struct nt_mouse_event mouse)
{
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_focus_scope_attach(ntg_focus_scope* scope);
