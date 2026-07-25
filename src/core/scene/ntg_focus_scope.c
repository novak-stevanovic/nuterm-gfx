#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static const struct ntg_focus_scope_vtable VTABLE_EMPTY = {0};

static void init_default(ntg_focus_scope* scope);
static bool handle_key_keybind(
        ntg_focus_scope* scope,
        struct nt_key_event key);

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

const struct ntg_focus_scope_keybinds NTG_FOCUS_SCOPE_KEYBINDS_EMPTY = {0};

const struct ntg_focus_scope_keybinds NTG_FOCUS_SCOPE_KEYBINDS_DEF = {
    .cancel_key = (struct nt_key_event) {
        .type = NT_KEY_EVENT_UTF32,
        .utf32 = {
            .alt = false,
            .cp = 27
        }
    },
    .left_click_key = (struct nt_key_event) {
        .type = NT_KEY_EVENT_UTF32,
        .utf32 = {
            .alt = false,
            .cp = 13
        }
    }
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

struct ntg_focus_scope_opts ntg_focus_scope_opts_def()
{
    return (struct ntg_focus_scope_opts) {
        .input_mode = NTG_FOCUS_SCOPE_INPUT_MODELESS,
        .out_click_mode = NTG_FOCUS_SCOPE_OUT_CLICK_KEEP,
        .block_mode = NTG_FOCUS_SCOPE_BLOCK_FALSE
    };
};

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_focus_scope_init(
        ntg_focus_scope* scope,
        ntg_object* scope_root,
        const struct ntg_focus_scope_keybinds* keybinds,
        const struct ntg_focus_scope_opts* opts,
        int* out_status)
{
    ntg_focus_scope_init_override(
            scope,
            &NTG_FOCUS_SCOPE_VTABLE_DEFAULT,
            scope_root,
            keybinds,
            opts,
            out_status);
}

void ntg_focus_scope_deinit(ntg_focus_scope* scope)
{
    if(!scope) return;

    init_default(scope);
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

    scope->_keybinds = (keybinds ? (*keybinds) : NTG_FOCUS_SCOPE_KEYBINDS_DEF);
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_focus_scope_feed_key(ntg_focus_scope* scope, struct nt_key_event key)
{
    if(!scope) return false;

    if(handle_key_keybind(scope, key))
        return true;

    if(scope->__vtable && scope->__vtable->handle_key_fn)
        return scope->__vtable->handle_key_fn(scope, key);
    else
        return false;
}

bool ntg_focus_scope_feed_mouse(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    if(!scope) return false;

    if(scope->__vtable && scope->__vtable->handle_mouse_fn)
        return scope->__vtable->handle_mouse_fn(scope, mouse, clicked);
    else
        return false;
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_focus_scope_init_override(
        ntg_focus_scope* scope,
        const struct ntg_focus_scope_vtable* vtable,
        ntg_object* scope_root,
        const struct ntg_focus_scope_keybinds* keybinds,
        const struct ntg_focus_scope_opts* opts,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!scope)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    init_default(scope);

    scope->__vtable = (vtable ? vtable : &VTABLE_EMPTY);
    scope->_root = scope_root;

    ntg_focus_scope_set_keybinds(scope, keybinds);
    ntg_focus_scope_set_opts(scope, opts);

    scope->__valid = true;
}

bool ntg_focus_scope_handle_key_fn(
        ntg_focus_scope* scope,
        struct nt_key_event key)
{
    if(!scope) return false;

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
        return ntg_object_feed_key(focused, key);
    else
        return false;
}

bool ntg_focus_scope_handle_key_bubble_fn(
        ntg_focus_scope* scope,
        struct nt_key_event key)
{
    if(!scope) return false;

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
    {
        ntg_object* it_obj = focused;
        while(it_obj)
        {
            if(ntg_object_feed_key(it_obj, key))
                return true;

            if(it_obj == scope->_root)
                break;

            it_obj = it_obj->_parent;
        }
        return false;
    }
    else
        return false;
}

bool ntg_focus_scope_handle_mouse_fn(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    if(!scope) return false;

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
    {
        if(clicked)
        {
            if(focused != clicked)
                ntg_focus_manager_request_focus(fm, clicked);

            if(clicked->_clickable)
            {
                return ntg_object_feed_mouse(clicked, mouse, NTG_OBJECT_CLICK_TRUE);
            }
        }
        else
        {
            ntg_focus_manager_request_focus(fm, NULL);
            return false;
        }
    }
    else
    {
        if(clicked)
        {
            ntg_focus_manager_request_focus(fm, clicked);

            if(clicked->_clickable)
            {
                return ntg_object_feed_mouse(clicked, mouse, NTG_OBJECT_CLICK_TRUE);
            }
            else
                return false;
        }
        else
        {
            return false;
        }
    }

    return false;
}

bool ntg_focus_scope_handle_mouse_bubble_fn(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    if(!scope) return false;

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
    {
        if(clicked)
        {
            if(focused != clicked)
                ntg_focus_manager_request_focus(fm, clicked);

            ntg_object* it_obj = clicked;
            while(it_obj)
            {
                if(it_obj->_clickable)
                {
                    if(ntg_object_feed_mouse(it_obj, mouse, NTG_OBJECT_CLICK_TRUE))
                        return true;
                }

                if(it_obj == scope->_root)
                    break;

                it_obj = it_obj->_parent;
            }

            return false;
        }
        else
        {
            ntg_focus_manager_request_focus(fm, NULL);
            return false;
        }
    }
    else
    {
        if(clicked)
        {
            ntg_focus_manager_request_focus(fm, clicked);

            ntg_object* it_obj = clicked;
            while(it_obj)
            {
                if(it_obj->_clickable)
                {
                    if(ntg_object_feed_mouse(it_obj, mouse, NTG_OBJECT_CLICK_TRUE))
                        return true;
                }

                if(it_obj == scope->_root)
                    break;

                it_obj = it_obj->_parent;
            }

            return false;
        }
        else
        {
            return false;
        }
    }
}

const struct ntg_focus_scope_vtable NTG_FOCUS_SCOPE_VTABLE_DEFAULT = {
    .handle_key_fn = ntg_focus_scope_handle_key_fn,
    .handle_mouse_fn = ntg_focus_scope_handle_mouse_fn
};

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_focus_scope_attach(ntg_focus_scope* scope, ntg_focus_manager* fm)
{
    if(!scope) return;

    scope->_fm = fm;
}

void _ntg_focus_scope_set_last_fcoused(ntg_focus_scope* scope, ntg_object* object)
{
    if(!scope) return;

    scope->_last_focused = object;
}

void _ntg_focus_scope_invalidate(ntg_focus_scope* scope)
{
    if(!scope) return;

    scope->__valid = false;
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_focus_scope* scope)
{
    (*scope) = (ntg_focus_scope) {0};
}

static bool handle_key_keybind(
        ntg_focus_scope* scope,
        struct nt_key_event key)
{
    if(!scope) return false;
    if(!scope->_fm) return false;
    if(key.type == NT_KEY_EVENT_UNBOUND) return false;

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
    {
        if(nt_key_event_are_eql(scope->_keybinds.left_click_key, key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_left(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_CLICK_KEYBIND);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.right_click_key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_right(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_CLICK_KEYBIND);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.middle_click_key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_middle(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_CLICK_KEYBIND);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.scroll_up_key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_scrollup(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_CLICK_KEYBIND);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.scroll_down_key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_scrolldwn(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_CLICK_KEYBIND);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.cancel_key))
        {
            ntg_focus_manager_request_focus(fm, NULL);
            return false;
        }

        return false;
    }
    else
        return false;
}
