#include "core/object/widget/focus/ntg_focus_mgr.h"
#include <assert.h>

void ntg_focus_mgr_init(ntg_focus_mgr* mgr, ntg_focus_mgr_process_fn process_fn)
{
    assert(mgr != NULL);
    assert(process_fn != NULL);

    mgr->__process_fn = process_fn;
}

void ntg_focus_mgr_deinit(ntg_focus_mgr* mgr)
{
    assert(mgr != NULL);

    mgr->__process_fn = NULL;
}

bool ntg_focus_mgr_feed_event(ntg_focus_mgr* mgr, struct ntg_event event)
{
    assert(mgr != NULL);

    return mgr->__process_fn(mgr, event);
}
