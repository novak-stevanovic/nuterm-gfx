#include "ntg.h"
#include "shared/ntg_shared_internal.h"

// TODO: mouse positions?

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
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.right_click_key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_right(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.middle_click_key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_middle(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.scroll_up_key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_scrollup(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.scroll_down_key))
        {
            struct nt_mouse_event mouse = nt_mouse_event_new_scrolldwn(0, 0);
            return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
        }
        else if(nt_key_event_are_eql(key, scope->_keybinds.cancel_key))
        {
            ntg_focus_manager_request_focus(fm, NULL);
            return true;
        }
        return ntg_object_feed_key(focused, key);
    }
    else
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
        bool (*handle_mouse_fn)(ntg_focus_scope* scope, struct nt_mouse_event mouse, ntg_object* clicked),
        int* out_status)
{
    ntg_init_status(out_status);

    if(!scope)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    scope->_root = scope_root;
    scope->_fm = NULL;

    scope->__handle_key_fn = handle_key_fn;
    scope->__handle_mouse_fn = handle_mouse_fn;

    ntg_focus_scope_set_keybinds(scope, keybinds);
    ntg_focus_scope_set_opts(scope, opts);

    scope->data = NULL;
}

void ntg_focus_scope_deinit(ntg_focus_scope* scope)
{
    if(!scope) return;

    scope->_fm = NULL;
    scope->_root = NULL;
    scope->_keybinds = (struct ntg_focus_scope_keybinds) {0};
    scope->_opts = ntg_focus_scope_opts_def();
    scope->__handle_key_fn = NULL;
    scope->__handle_mouse_fn = NULL;
    scope->data = NULL;
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

bool ntg_focus_scope_feed_mouse(
        ntg_focus_scope* scope,
        struct nt_mouse_event mouse,
        ntg_object* clicked)
{
    if(!scope) return false;

    return scope->__handle_mouse_fn(scope, mouse, clicked);
}

/* ------------------------------------------------------ */
/* DEFAULT VIRTUAL FUNCTIONS */
/* ------------------------------------------------------ */

bool ntg_focus_scope_handle_key_fn(ntg_focus_scope* scope, struct nt_key_event key)
{
    if(!scope) return false; 

    ntg_focus_manager* fm = scope->_fm;
    ntg_object* focused = fm->_focused;

    if(focused)
        return ntg_object_feed_key(focused, key);
    else
        return false;
}

bool ntg_focus_scope_handle_key_bubble_fn(ntg_focus_scope* scope, struct nt_key_event key)
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
                ntg_focus_manager_request_focus(fm, NULL);

            if(clicked->_clickable)
                return ntg_object_feed_mouse(clicked, mouse, NTG_OBJECT_MOUSE_TRUE);
        }
        else
        {
            ntg_focus_manager_request_focus(fm, NULL);
            return true;
        }
    }
    else
    {
        if(clicked)
        {
            ntg_focus_manager_request_focus(fm, clicked);

            if(clicked->_clickable)
                ntg_object_feed_mouse(clicked, mouse, NTG_OBJECT_MOUSE_TRUE);

            return true;
        }
        else
        {
            return false;
        }
    }
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
            bool consumed = false;
            if(focused != clicked)
                ntg_focus_manager_request_focus(fm, NULL); // TODO: return true?

            ntg_object* it_obj = focused;
            while(it_obj)
            {
                if(it_obj->_clickable)
                {
                    if(ntg_object_feed_mouse(it_obj, mouse, NTG_OBJECT_MOUSE_TRUE))
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
            return true;
        }
    }
    else
    {
        if(clicked)
        {
            ntg_focus_manager_request_focus(fm, clicked); // TODO: return true?

            ntg_object* it_obj = focused;
            while(it_obj)
            {
                if(it_obj->_clickable)
                {
                    if(ntg_object_feed_mouse(it_obj, mouse, NTG_OBJECT_MOUSE_TRUE))
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

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_focus_scope_attach(ntg_focus_scope* scope)
{
    if(!scope) return;

    ntg_scene* root_scene = ntg_object_get_scene_(scope->_root);
    if(!root_scene) return;

    scope->_fm = root_scene->_fm;
}
