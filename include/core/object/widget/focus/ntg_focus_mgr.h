#ifndef NTG_FOCUS_MGR_H
#define NTG_FOCUS_MGR_H

#include "shared/ntg_typedef.h"
#include "core/entity/ntg_entity.h"
#include "nt_event.h"

struct ntg_focus_mgr
{
    ntg_entity __base;

    ntg_widget* _focused;
    
    struct
    {
        bool (*on_key_fn)(ntg_focus_mgr* mgr, struct nt_key_event key);
        bool (*on_mouse_fn)(ntg_focus_mgr* mgr, struct nt_mouse_event mouse);
    };
};

void ntg_focus_mgr_init(ntg_focus_mgr* mgr);
void ntg_focus_mgr_deinit(ntg_focus_mgr* mgr);

// void ntg_focus_mgr_focus

bool ntg_focus_mgr_dispatch_key(ntg_focus_mgr* mgr, struct nt_key_event key);
bool ntg_focus_mgr_dispatch_mouse(ntg_focus_mgr* mgr, struct nt_mouse_event mouse);

#endif // NTG_FOCUS_MGR_H
