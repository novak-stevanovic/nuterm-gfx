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

static bool handle_key_keybind(ntg_focus_scope* scope, struct nt_key_event key);
static void handle_mouse_focus(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked);

static inline bool is_bound(struct ntg_focus_scope_keybind kb, struct nt_key_event key)
{
    return (kb.bound && nt_key_event_are_eql(kb.key, key));
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_focus_scope_opts ntg_focus_scope_opts_def()
{
    return (struct ntg_focus_scope_opts) {
        .input_mode = NTG_FOCUS_SCOPE_INPUT_MODELESS,
        .out_click_mode = NTG_FOCUS_SCOPE_OUT_CLICK_CLR,
        .block_mode = NTG_FOCUS_SCOPE_BLOCK_FALSE,
        .keybind_mode = NTG_FOCUS_SCOPE_KEYBIND_FIRST,

    };
};

const struct ntg_focus_scope_keybinds NTG_FOCUS_SCOPE_KEYBINDS_DEFAULT = {
    .cancel = {
        true,
        (struct nt_key_event) {
            .type = NT_KEY_EVENT_UTF32,
            .utf32 = {
                .alt = false,
                .cp = 27
            }
        }
    },
    .left_click = {
        true,
        (struct nt_key_event) {
            .type = NT_KEY_EVENT_UTF32,
            .utf32 = {
                .alt = false,
                .cp = 13 
            }
        }
    }
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

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

    scope->_keybinds = (keybinds ? (*keybinds) : NTG_FOCUS_SCOPE_KEYBINDS_DEFAULT);
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_focus_scope_feed_key(ntg_focus_scope* scope, struct nt_key_event key)
{
    if(!scope) return false;

    if(scope->_opts.keybind_mode == NTG_FOCUS_SCOPE_KEYBIND_FIRST)
    {
        if(handle_key_keybind(scope, key))
            return true;

        if(scope->__vtable && scope->__vtable->dispatch_key_fn)
            return scope->__vtable->dispatch_key_fn(scope, key);
        else
            return false;
    }
    else
    {
        if(scope->__vtable && scope->__vtable->dispatch_key_fn)
            return scope->__vtable->dispatch_key_fn(scope, key);

        if(handle_key_keybind(scope, key))
            return true;
        else
            return false;
    }
}

bool ntg_focus_scope_feed_mouse(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    if(!scope) return false;

    handle_mouse_focus(scope, mouse, clicked);

    if(scope->__vtable && scope->__vtable->dispatch_mouse_fn)
        return scope->__vtable->dispatch_mouse_fn(scope, mouse, clicked);
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

bool ntg_focus_scope_dispatch_key_fn(
        ntg_focus_scope* scope,
        struct nt_key_event key)
{
    if(!scope) return false;

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
    {
        struct ntg_object_key event = {
            .key = key,
            .target = focused
        };
        return ntg_object_feed_key(focused, &event);
    }
    else
        return false;
}

bool ntg_focus_scope_dispatch_key_bubble_fn(
        ntg_focus_scope* scope,
        struct nt_key_event key)
{
    if(!scope) return false;

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
    {
        struct ntg_object_key event = {
            .key = key,
            .target = focused
        };

        ntg_object* it_obj = focused;
        while(it_obj)
        {
            if(ntg_object_feed_key(it_obj, &event))
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

bool ntg_focus_scope_dispatch_mouse_fn(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    if(!scope) return false;
    if(!clicked) return false;

    struct ntg_object_mouse event = {
        .mouse = mouse,
        .target = clicked,
        .from_keybind = false
    };

    // Pass event
    if(clicked->_clickable)
    {
        return ntg_object_feed_mouse(clicked, &event);
    }

    return false;
}

bool ntg_focus_scope_dispatch_mouse_bubble_fn(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    if(!scope) return false;
    if(!clicked) return false;

    struct ntg_object_mouse event = {
        .mouse = mouse,
        .target = clicked,
        .from_keybind = false
    };

    // Bubble event
    ntg_object* it_obj = clicked;
    while(it_obj)
    {
        if(it_obj->_clickable)
        {
            if(ntg_object_feed_mouse(it_obj, &event))
                return true;
        }

        if(it_obj == scope->_root)
            break;

        it_obj = it_obj->_parent;
    }

    return false;
}

const struct ntg_focus_scope_vtable NTG_FOCUS_SCOPE_VTABLE_DEFAULT = {
    .dispatch_key_fn = ntg_focus_scope_dispatch_key_fn,
    .dispatch_mouse_fn = ntg_focus_scope_dispatch_mouse_fn
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

void _ntg_focus_scope_set_last_focused(ntg_focus_scope* scope, ntg_object* object)
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

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
    {
        struct ntg_object_mouse event = {
            .mouse = {0},
            .target = focused,
            .from_keybind = true
        };

        if(is_bound(scope->_keybinds.left_click, key))
        {
            event.mouse.type = NT_MOUSE_EVENT_CLICK_LEFT;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->_keybinds.right_click, key))
        {
            event.mouse.type = NT_MOUSE_EVENT_CLICK_RIGHT;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->_keybinds.middle_click, key))
        {
            event.mouse.type = NT_MOUSE_EVENT_CLICK_MIDDLE;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->_keybinds.scroll_up, key))
        {
            event.mouse.type = NT_MOUSE_EVENT_SCROLL_UP;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->_keybinds.scroll_down, key))
        {
            event.mouse.type = NT_MOUSE_EVENT_SCROLL_DOWN;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->_keybinds.cancel, key))
        {
            ntg_focus_manager_request_focus(fm, NULL);
            return false;
        }

        return false;
    }
    else
        return false;
}

static void handle_mouse_focus(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    // Decide new focus
    if(focused)
    {
        if(clicked)
        {
            if(clicked->_focusable)
            {
                if(focused != clicked)
                {
                    if(mouse.type == NT_MOUSE_EVENT_CLICK_LEFT)
                        ntg_focus_manager_request_focus(fm, clicked);
                    else
                        ntg_focus_manager_request_focus(fm, NULL);
                }
            }
            else
                ntg_focus_manager_request_focus(fm, NULL);
        }
        else
            ntg_focus_manager_request_focus(fm, NULL);
    }
    else
    {
        if(clicked && clicked->_focusable)
        {
            if(mouse.type == NT_MOUSE_EVENT_CLICK_LEFT)
                ntg_focus_manager_request_focus(fm, clicked);
        }
    }
}
