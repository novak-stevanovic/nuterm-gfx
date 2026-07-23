#include "ntg.h"

bool ntg_focus_scope_dispatch_key(
        void* _,
        const struct ntg_focus_key_ctx* ctx,
        struct nt_key_event key,
        const struct ntg_focus_scope_keybinds* keybinds)
{
    if(!ctx) return false;
    if(!ctx->fm) return false;

    ntg_object* focused = ctx->fm->_focused;

    if(focused)
    {
        if(keybinds)
        {
            if(nt_key_event_are_eql(keybinds->left_click_key, key))
            {
                struct nt_mouse_event mouse = nt_mouse_event_new_left(0, 0);
                return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
            }
            else if(nt_key_event_are_eql(key, keybinds->right_click_key))
            {
                struct nt_mouse_event mouse = nt_mouse_event_new_right(0, 0);
                return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
            }
            else if(nt_key_event_are_eql(key, keybinds->middle_click_key))
            {
                struct nt_mouse_event mouse = nt_mouse_event_new_middle(0, 0);
                return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
            }
            else if(nt_key_event_are_eql(key, keybinds->scroll_up_key))
            {
                struct nt_mouse_event mouse = nt_mouse_event_new_scrollup(0, 0);
                return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
            }
            else if(nt_key_event_are_eql(key, keybinds->scroll_down_key))
            {
                struct nt_mouse_event mouse = nt_mouse_event_new_scrolldwn(0, 0);
                return ntg_object_feed_mouse(focused, mouse, NTG_OBJECT_MOUSE_SCENE);
            }
            else if(nt_key_event_are_eql(key, keybinds->cancel_key))
            {
                ntg_focus_manager_request_focus(ctx->fm, NULL);
                return true;
            }
        }
        return ntg_object_feed_key(focused, key);
    }
    else
        return false;
}

bool ntg_focus_scope_dispatch_mouse_stc(
        void* _,
        const struct ntg_focus_mouse_ctx* ctx,
        struct nt_mouse_event mouse)
{
    if(!ctx) return false;
    if(!ctx->fm) return false;

    if(ctx->clicked && ctx->clicked->_clickable)
        return ntg_object_feed_mouse(ctx->clicked, mouse, NTG_OBJECT_MOUSE_TRUE);
    else
        return false;
}

bool ntg_focus_scope_dispatch_mouse_dyn(
        void* _,
        const struct ntg_focus_mouse_ctx* ctx,
        struct nt_mouse_event mouse)
{
    if(!ctx) return false;
    if(!ctx->fm) return false;

    ntg_object* focused = ctx->fm->_focused;

    if(focused)
    {
        if(ctx->clicked)
        {
            if((focused == ctx->clicked) && (ctx->clicked->_clickable))
            {
                return ntg_object_feed_mouse(ctx->clicked, mouse, NTG_OBJECT_MOUSE_TRUE);
            }
            else
            {
                ntg_focus_manager_request_focus(ctx->fm, NULL);
                ntg_object_feed_mouse(ctx->clicked, mouse, NTG_OBJECT_MOUSE_TRUE);
                return true;
            }
        }
        else
        {
            ntg_focus_manager_request_focus(ctx->fm, NULL);
            return true;
        }
    }
    else
    {
        if(ctx->clicked)
        {
            ntg_focus_manager_request_focus(ctx->fm, ctx->clicked);

            if(ctx->clicked->_clickable)
                ntg_object_feed_mouse(ctx->clicked, mouse, NTG_OBJECT_MOUSE_TRUE);

            return true;
        }
        else
        {
            return false;
        }
    }
}
