#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* KEY */
/* ------------------------------------------------------ */

bool ntg_fcs_scope_key_ignore_fn(const struct ntg_fcs_scope_ctx* ctx, nt_key key)
{
    (void)ctx;
    (void)key;

    return false;
}

bool ntg_fcs_scope_key_dispatch_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key)
{
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_widget* focused = fm->ro.focused;

    if(focused)
    {
        struct ntg_widget_key event = {
            .key = key,
            .target = focused
        };
        return ntg_widget_feed_key(focused, &event);
    }
    else
        return false;
}

bool ntg_fcs_scope_key_bubble_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key)
{
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_widget* focused = fm->ro.focused;

    if(focused)
    {
        struct ntg_widget_key event = {
            .key = key,
            .target = focused
        };

        ntg_widget* it_obj = focused;
        while(it_obj)
        {
            if(ntg_widget_feed_key(it_obj, &event))
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

/* ------------------------------------------------------ */
/* MOUSE */
/* ------------------------------------------------------ */

bool ntg_fcs_scope_mouse_ignore_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_widget* clicked)
{
    (void)ctx;
    (void)mouse;
    (void)clicked;

    return false;
}

bool ntg_fcs_scope_mouse_dispatch_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_widget* clicked)
{
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return false;
    if(!clicked) return false;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_widget* focused = fm->ro.focused;

    bool click = (mouse.type == NT_MOUSE_CLICK_LEFT) ||
            (mouse.type == NT_MOUSE_CLICK_RIGHT) ||
            (mouse.type == NT_MOUSE_CLICK_MIDDLE);

    ntg_widget* target = (click ? clicked : focused);

    if(!click && !focused)
        return false;

    struct ntg_widget_mouse event = {
        .mouse = mouse,
        .target = target,
        .from_keybind = false
    };

    // Pass event
    if(target->ro.clickable)
    {
        return ntg_widget_feed_mouse(target, &event);
    }

    return false;
}

bool ntg_fcs_scope_mouse_bubble_fn(
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_widget* clicked)
{
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return false;
    if(!clicked) return false;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_widget* focused = fm->ro.focused;

    bool click = (mouse.type == NT_MOUSE_CLICK_LEFT) ||
            (mouse.type == NT_MOUSE_CLICK_RIGHT) ||
            (mouse.type == NT_MOUSE_CLICK_MIDDLE);
    
    if(!click && !focused)
        return false;

    ntg_widget* target = (click ? clicked : focused);

    struct ntg_widget_mouse event = {
        .mouse = mouse,
        .target = target,
        .from_keybind = false
    };

    // Bubble event
    ntg_widget* it_obj = target;
    while(it_obj)
    {
        if(it_obj->ro.clickable)
        {
            if(ntg_widget_feed_mouse(it_obj, &event))
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

static const struct ntg_fcs_scope_keys KEYS_AUTO = {
    .left_click = {
        .set = true,
        .type = NT_KEY_UTF32,
        .data.utf32 = {
            .alt = false,
            .cp = 13
        }
    },
    .cancel = {
        .set = true,
        .type = NT_KEY_UTF32,
        .data.utf32 = {
            .alt = false,
            .cp = 27
        }
    }
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

bool ntg__fcs_scope_handle_keybind(
        const struct ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_ctx* ctx,
        nt_key key)
{
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    struct ntg_fcs_scope_keys keys_final = KEYS_AUTO;
    if(scope->keys.set) keys_final = scope->keys;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_widget* focused = fm->ro.focused;

    if(focused)
    {
        struct ntg_widget_mouse event = {
            .mouse = {0},
            .target = focused,
            .from_keybind = true
        };

        if(nt_key_are_eql(keys_final.left_click, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_CLICK_LEFT, 0, 0);
            return ntg_widget_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(keys_final.right_click, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_CLICK_RIGHT, 0, 0);
            return ntg_widget_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(keys_final.middle_click, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_CLICK_MIDDLE, 0, 0);
            return ntg_widget_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(keys_final.scroll_up, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_SCROLL_UP, 0, 0);
            return ntg_widget_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(keys_final.scroll_down, key))
        {
            event.mouse = nt_mouse_new(NT_MOUSE_SCROLL_DOWN, 0, 0);
            return ntg_widget_feed_mouse(focused, &event);
        }
        else if(nt_key_are_eql(keys_final.cancel, key))
        {
            ntg_fcs_manager_request_focus(fm, NULL);
            return false;
        }

        return false;
    }
    else
        return false;
}

static const uint8_t mouse_flags_default[5] = {
    NTG_FCS_SCOPE_MOUSE_CAN_UNFCS ^ NTG_FCS_SCOPE_MOUSE_CAN_FCS,
    NTG_FCS_SCOPE_MOUSE_CAN_UNFCS ^ NTG_FCS_SCOPE_MOUSE_CAN_FCS,
    NTG_FCS_SCOPE_MOUSE_CAN_UNFCS,
    0,
    0
};

void ntg__fcs_scope_handle_mouse_focus(
        const struct ntg_fcs_scope* scope,
        const struct ntg_fcs_scope_ctx* ctx,
        nt_mouse mouse,
        ntg_widget* clicked)
{
    if(!scope || !ctx) return;
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return;

    ntg_fcs_manager* fm = ctx->fm;
    ntg_widget* focused = fm->ro.focused;

    uint8_t flag = 0;

    if(mouse.type <= 4)
    {
        flag = ntg_uint8_opt_get(
                scope->mouse_flags[mouse.type],
                mouse_flags_default[mouse.type]);
    }

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
