#include "ntg.h"

bool ntg_focus_scope_dispatch_key(
        void* data,
        const struct ntg_focus_key_ctx* ctx,
        struct nt_key_event key)
{
    if(!ctx) return false;
    if(!ctx->fm) return false;

    ntg_object* focused = ctx->fm->_focused;

    if(focused)
        return ntg_object_on_key(focused, key);
    else
        return false;
}

bool ntg_focus_scope_dispatch_mouse_static(
        void* data,
        const struct ntg_focus_mouse_ctx* ctx,
        struct nt_mouse_event mouse)
{
    if(!ctx) return false;
    if(!ctx->fm) return false;

    if(ctx->clicked)
        return ntg_object_on_mouse(ctx->clicked, mouse);
    else
        return false;
}

bool ntg_focus_scope_dispatch_mouse_dynamic(
        void* data,
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
            if(focused == ctx->clicked)
            {
                return ntg_object_on_mouse(ctx->clicked, mouse);
            }
            else
            {
                ntg_focus_manager_request_focus(ctx->fm, NULL);
                ntg_object_on_mouse(ctx->clicked, mouse);
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
            ntg_object_on_mouse(ctx->clicked, mouse);
            return true;
        }
        else
        {
            return false;
        }
    }
}
