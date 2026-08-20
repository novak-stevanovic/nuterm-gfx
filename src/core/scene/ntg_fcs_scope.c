#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static const struct ntg_fcs_scope_vtable VTABLE_EMPTY = {0};

static void init_default(ntg_fcs_scope* scope);

static bool handle_key_keybind(ntg_fcs_scope* scope, struct nt_key key);
static void handle_mouse_focus(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked);

static inline bool is_bound(struct ntg_fcs_scope_key kb, struct nt_key key)
{
    return (kb.bound && nt_key_are_eql(kb.key, key));
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_fcs_scope_opts ntg_fcs_scope_opts_default(void)
{
     struct ntg_fcs_scope_opts opts = {
        .input_mode = NTG_FCS_SCOPE_INPUT_MODELESS,
        .out_click_mode = NTG_FCS_SCOPE_OUT_CLICK_CLR,
        .block_mode = NTG_FCS_SCOPE_BLOCK_FALSE,
        .mouse_flags = {0, 0, 0, 0, 0}
    };

    opts.mouse_flags[NT_MOUSE_CLICK_LEFT] ^= (NTG_FCS_SCOPE_MOUSE_CAN_UNFCS ^
                                              NTG_FCS_SCOPE_MOUSE_CAN_FCS);

    opts.mouse_flags[NT_MOUSE_CLICK_RIGHT] ^= (NTG_FCS_SCOPE_MOUSE_CAN_UNFCS ^
                                               NTG_FCS_SCOPE_MOUSE_CAN_FCS);

    opts.mouse_flags[NT_MOUSE_CLICK_MIDDLE] ^= NTG_FCS_SCOPE_MOUSE_CAN_UNFCS;

    return opts;
}

struct ntg_fcs_scope_key ntg_fcs_scope_key_new(struct nt_key key)
{
    return (struct ntg_fcs_scope_key) { .bound = true, .key = key };
}

const struct ntg_fcs_scope_keys NTG_FCS_SCOPE_KEYS_AUTO = {
    .cancel = {
        true,
        {
            .type = NT_KEY_UTF32,
            .data = { .utf32 = { .alt = false, .cp = 27 } }
        }
    },
    .left_click = {
        true,
        {
            .type = NT_KEY_UTF32,
            .data = { .utf32 = { .alt = false, .cp = 13 } }
        }
    }
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_fcs_scope_init(
        ntg_fcs_scope* scope,
        ntg_object* scope_root,
        const struct ntg_fcs_scope_keys* keybinds,
        const struct ntg_fcs_scope_opts* opts)
{
    return ntg_fcs_scope_init_inherit(
            scope,
            &NTG_FCS_SCOPE_VTABLE_DEFAULT,
            scope_root,
            keybinds,
            opts);
}

int ntg_fcs_scope_deinit(ntg_fcs_scope* scope)
{
    if(!scope) return NTG_ERR_INV_ARG;

    init_default(scope);

    return 0;
}

/* ------------------------------------------------------ */
/* SETTERS */
/* ------------------------------------------------------ */

int ntg_fcs_scope_set_opts(
        ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_opts* opts)
{
    if(!scope) return NTG_ERR_INV_ARG;

    scope->ro.opts = (opts ? (*opts) : ntg_fcs_scope_opts_default());

    return 0;
}

int ntg_fcs_scope_set_keys(
        ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_keys* keybinds)
{
    if(!scope) return NTG_ERR_INV_ARG;

    scope->ro.keys = (keybinds ? (*keybinds) : NTG_FCS_SCOPE_KEYS_AUTO);

    return 0;
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_fcs_scope_feed_key(ntg_fcs_scope* scope, struct nt_key key)
{
    if(!scope) return false;

    if(handle_key_keybind(scope, key))
        return true;

    if(scope->priv.vtable && scope->priv.vtable->dispatch_key_fn)
        return scope->priv.vtable->dispatch_key_fn(scope, key);

    return false;
}

bool ntg_fcs_scope_feed_mouse(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked)
{
    if(!scope) return false;

    handle_mouse_focus(scope, mouse, clicked);

    if(scope->priv.vtable && scope->priv.vtable->dispatch_mouse_fn)
        return scope->priv.vtable->dispatch_mouse_fn(scope, mouse, clicked);
    else
        return false;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_fcs_scope_init_inherit(
        ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_vtable* vtable,
        ntg_object* scope_root,
        const struct ntg_fcs_scope_keys* keybinds,
        const struct ntg_fcs_scope_opts* opts)
{
    if(!scope)
        return NTG_ERR_INV_ARG;

    init_default(scope);

    scope->priv.vtable = (vtable ? vtable : &VTABLE_EMPTY);
    scope->ro.root = scope_root;

    ntg_fcs_scope_set_keys(scope, keybinds);
    ntg_fcs_scope_set_opts(scope, opts);

    scope->priv.valid = true;
    return 0;
}

bool ntg_fcs_scope_dispatch_key_fn(
        ntg_fcs_scope* scope,
        struct nt_key key)
{
    if(!scope) return false;

    ntg_fcs_manager* fm = scope->ro.fm;
    ntg_object* focused = fm->ro.focused;

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

bool ntg_fcs_scope_dispatch_key_bubble_fn(
        ntg_fcs_scope* scope,
        struct nt_key key)
{
    if(!scope) return false;

    ntg_fcs_manager* fm = scope->ro.fm;
    ntg_object* focused = fm->ro.focused;

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

            if(it_obj == scope->ro.root)
                break;

            it_obj = it_obj->ro.parent;
        }
        return false;
    }
    else
        return false;
}

bool ntg_fcs_scope_dispatch_mouse_fn(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked)
{
    if(!scope) return false;
    if(!clicked) return false;
    if(!scope->ro.fm) return false;

    bool click = (mouse.type == NT_MOUSE_CLICK_LEFT) ||
            (mouse.type == NT_MOUSE_CLICK_RIGHT) ||
            (mouse.type == NT_MOUSE_CLICK_MIDDLE);

    ntg_object* target = (click ? clicked : scope->ro.fm->ro.focused);

    if(!click && !scope->ro.fm->ro.focused)
        return false;

    struct ntg_object_mouse event = {
        .mouse = mouse,
        .target = target,
        .from_keybind = false
    };

    // Pass event
    if(target->ro.clickable)
    {
        return ntg_object_feed_mouse(target, &event);
    }

    return false;
}

bool ntg_fcs_scope_dispatch_mouse_bubble_fn(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked)
{
    if(!scope) return false;
    if(!clicked) return false;
    if(!scope->ro.fm) return false;

    bool click = (mouse.type == NT_MOUSE_CLICK_LEFT) ||
            (mouse.type == NT_MOUSE_CLICK_RIGHT) ||
            (mouse.type == NT_MOUSE_CLICK_MIDDLE);
    
    if(!click && !scope->ro.fm->ro.focused)
        return false;

    ntg_object* target = (click ? clicked : scope->ro.fm->ro.focused);

    struct ntg_object_mouse event = {
        .mouse = mouse,
        .target = target,
        .from_keybind = false
    };

    // Bubble event
    ntg_object* it_obj = target;
    while(it_obj)
    {
        if(it_obj->ro.clickable)
        {
            if(ntg_object_feed_mouse(it_obj, &event))
                return true;
        }

        if(it_obj == scope->ro.root)
            break;

        it_obj = it_obj->ro.parent;
    }

    return false;
}

const struct ntg_fcs_scope_vtable NTG_FCS_SCOPE_VTABLE_DEFAULT = {
    .dispatch_key_fn = ntg_fcs_scope_dispatch_key_fn,
    .dispatch_mouse_fn = ntg_fcs_scope_dispatch_mouse_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

void ntg__fcs_scope_attach(ntg_fcs_scope* scope, ntg_fcs_manager* fm)
{
    if(!scope) return;

    scope->ro.fm = fm;

}

void ntg__fcs_scope_set_last_focused(ntg_fcs_scope* scope, ntg_object* object)
{
    if(!scope) return;

    scope->ro.last_focused = object;

}

void ntg__fcs_scope_invalidate(ntg_fcs_scope* scope)
{
    if(!scope) return;

    scope->priv.valid = false;

}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void init_default(ntg_fcs_scope* scope)
{
    (*scope) = (ntg_fcs_scope) {0};
    scope->ro.keys = NTG_FCS_SCOPE_KEYS_AUTO;
    scope->ro.opts = ntg_fcs_scope_opts_default();
}

static bool handle_key_keybind(
        ntg_fcs_scope* scope,
        struct nt_key key)
{
    if(!scope) return false;
    if(!scope->ro.fm) return false;

    ntg_fcs_manager* fm = scope->ro.fm;
    ntg_object* focused = fm->ro.focused;

    if(focused)
    {
        struct ntg_object_mouse event = {
            .mouse = {0},
            .target = focused,
            .from_keybind = true
        };

        if(is_bound(scope->ro.keys.left_click, key))
        {
            event.mouse.type = NT_MOUSE_CLICK_LEFT;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->ro.keys.right_click, key))
        {
            event.mouse.type = NT_MOUSE_CLICK_RIGHT;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->ro.keys.middle_click, key))
        {
            event.mouse.type = NT_MOUSE_CLICK_MIDDLE;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->ro.keys.scroll_up, key))
        {
            event.mouse.type = NT_MOUSE_SCROLL_UP;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->ro.keys.scroll_down, key))
        {
            event.mouse.type = NT_MOUSE_SCROLL_DOWN;
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(is_bound(scope->ro.keys.cancel, key))
        {
            ntg_fcs_manager_request_focus(fm, NULL);
            return false;
        }

        return false;
    }
    else
        return false;
}

static void handle_mouse_focus(
        ntg_fcs_scope* scope,
        struct nt_mouse mouse,
        ntg_object* clicked)
{
    ntg_fcs_manager* fm = scope->ro.fm;
    ntg_object* focused = fm->ro.focused;

    uint8_t flag = (mouse.type <= 4) ?
        scope->ro.opts.mouse_flags[mouse.type] :
        0;

    // Decide new focus
    if(focused)
    {
        if(clicked)
        {
            if(clicked->ro.focusable)
            {
                if(focused != clicked)
                {
                    if(flag & NTG_FCS_SCOPE_MOUSE_CAN_FCS)
                        ntg_fcs_manager_request_focus(fm, clicked);
                    else if(flag & NTG_FCS_SCOPE_MOUSE_CAN_UNFCS)
                        ntg_fcs_manager_request_focus(fm, NULL);
                }
            }
            else
            {
                if(flag & NTG_FCS_SCOPE_MOUSE_CAN_UNFCS)
                    ntg_fcs_manager_request_focus(fm, NULL);
            }
        }
        else
        {
            if(flag & NTG_FCS_SCOPE_MOUSE_CAN_UNFCS)
                ntg_fcs_manager_request_focus(fm, NULL);
        }
    }
    else
    {
        if(clicked && clicked->ro.focusable)
        {
            if(flag & NTG_FCS_SCOPE_MOUSE_CAN_FCS)
                ntg_fcs_manager_request_focus(fm, clicked);
            else if(flag & NTG_FCS_SCOPE_MOUSE_CAN_UNFCS)
                ntg_fcs_manager_request_focus(fm, NULL);
        }
    }
}
