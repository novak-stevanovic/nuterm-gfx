#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/*
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

struct ntg_fcs_scope_keys ntg_fcs_scope_keys_default(void)
{
    return (struct ntg_fcs_scope_keys) {
        .cancel = nt_key_utf32_new(27, false),
        .left_click = nt_key_utf32_new(13, false)
    };
}
*/

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

bool ntg_fcs_scope_dispatch_key_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key)
{
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    ntg_fcs_manager* fm = ctx->fm;
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
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key)
{
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    ntg_fcs_manager* fm = ctx->fm;
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

            if(it_obj == ctx->root)
                break;

            it_obj = it_obj->ro.parent;
        }
        return false;
    }
    else
        return false;
}

bool ntg_fcs_scope_dispatch_mouse_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_object* clicked)
{
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return false;
    if(!clicked) return false;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_object* focused = fm->ro.focused;

    bool click = (mouse.type == NT_MOUSE_CLICK_LEFT) ||
            (mouse.type == NT_MOUSE_CLICK_RIGHT) ||
            (mouse.type == NT_MOUSE_CLICK_MIDDLE);

    ntg_object* target = (click ? clicked : focused);

    if(!click && !focused)
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
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_object* clicked)
{
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return false;
    if(!clicked) return false;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_object* focused = fm->ro.focused;

    bool click = (mouse.type == NT_MOUSE_CLICK_LEFT) ||
            (mouse.type == NT_MOUSE_CLICK_RIGHT) ||
            (mouse.type == NT_MOUSE_CLICK_MIDDLE);
    
    if(!click && !focused)
        return false;

    ntg_object* target = (click ? clicked : focused);

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

        if(it_obj == ctx->root)
            break;

        it_obj = it_obj->ro.parent;
    }

    return false;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

bool ntg__fcs_scope_handle_keybind(
        const struct ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key)
{
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_object* focused = fm->ro.focused;

    if(focused)
    {
        struct ntg_object_mouse event = {
            .mouse = {0},
            .target = focused,
            .from_keybind = true
        };

        if(nt_key_are_eql(scope->keys.left_click, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_CLICK_LEFT, 0, 0);
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(scope->keys.right_click, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_CLICK_RIGHT, 0, 0);
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(scope->keys.middle_click, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_CLICK_MIDDLE, 0, 0);
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(scope->keys.scroll_up, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_SCROLL_UP, 0, 0);
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(scope->keys.scroll_down, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_SCROLL_DOWN, 0, 0);
            return ntg_object_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(scope->keys.cancel, key))
        {
            ntg_fcs_manager_request_focus(fm, NULL);
            return false;
        }

        return false;
    }
    else
        return false;
}

void ntg__fcs_scope_handle_mouse_focus(
        const struct ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_object* clicked)
{
    if(!scope || !ctx) return;
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_object* focused = fm->ro.focused;

    uint8_t flag = (mouse.type <= 4) ?
        scope->opts.mouse_flags[mouse.type] :
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
