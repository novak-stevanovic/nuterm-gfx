#include "core/object/widget/focus/ntg_focus_mgr.h"
#include "ntg.h"
#include <assert.h>

void ntg_focus_mgr_init(ntg_focus_mgr* mgr)
{
    assert(mgr != NULL);

    mgr->on_key_fn = ntg_focus_mgr_dispatch_key;
    mgr->on_mouse_fn = ntg_focus_mgr_dispatch_mouse;
}

void ntg_focus_mgr_deinit(ntg_focus_mgr* mgr)
{
    assert(mgr != NULL);

    mgr->on_key_fn = ntg_focus_mgr_dispatch_key;
    mgr->on_mouse_fn = ntg_focus_mgr_dispatch_mouse;
}

bool ntg_focus_mgr_dispatch_key(ntg_focus_mgr* mgr, struct nt_key_event key)
{
    assert(mgr != NULL);

    if(mgr->_focused)
    {
        ntg_widget* focused = mgr->_focused;
        if(focused->on_key_fn)
            return focused->on_key_fn(focused, key);
        else
            return false;
    }
    else
        return false;
}

bool ntg_focus_mgr_dispatch_mouse(ntg_focus_mgr* mgr, struct nt_mouse_event mouse)
{
    assert(mgr != NULL);

    if(mgr->_focused)
    {
        ntg_widget* focused = mgr->_focused;
        if(focused->on_mouse_fn)
            return focused->on_mouse_fn(focused, mouse);
        else
            return false;
    }
    else
        return false;
}
