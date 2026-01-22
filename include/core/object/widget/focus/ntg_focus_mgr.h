#ifndef NTG_FOCUS_MGR_H
#define NTG_FOCUS_MGR_H

#include "shared/ntg_typedef.h"
#include "core/entity/ntg_entity.h"

struct ntg_focus_mgr
{
    ntg_entity __base;
    ntg_focus_mgr_process_fn __process_fn;
};

void ntg_focus_mgr_init(ntg_focus_mgr* mgr, ntg_focus_mgr_process_fn process_fn);
void ntg_focus_mgr_deinit(ntg_focus_mgr* mgr);

bool ntg_focus_mgr_feed_event(ntg_focus_mgr* mgr, struct ntg_event event,
                              ntg_loop_ctx* ctx);

#endif // NTG_FOCUS_MGR_H
