#include "ntg.h"

bool ntg_focus_scope_dispatch_key(
        void* _,
        const struct ntg_focus_key_ctx* ctx,
        struct nt_key_event key,
        const struct ntg_focus_scope_keybinds* keybinds)
{
    if(!ctx) return false;
    if(!ctx->fm) return false;

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
}
