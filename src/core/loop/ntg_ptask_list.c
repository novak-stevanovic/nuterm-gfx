#include "core/loop/ntg_ptask_list.h"
#include "shared/ntg_fwd_list.h"
#include "core/loop/_ntg_loop.h"
#include <assert.h>

void ntg_ptask_list_init(ntg_ptask_list* list)
{
    assert(list != NULL);

    ntg_fwd_list_init((ntg_fwd_list*)list);
}
void ntg_ptask_list_deinit(ntg_ptask_list* list)
{
    assert(list != NULL);

    ntg_fwd_list_deinit((ntg_fwd_list*)list);
}

void ntg_ptask_list_push_front(ntg_ptask_list* list, struct ntg_ptask ptask)
{
    assert(list != NULL);

    ntg_fwd_list_push_front((ntg_fwd_list*)list, &ptask, sizeof(struct ntg_ptask));
}

void ntg_ptask_list_push_back(ntg_ptask_list* list, struct ntg_ptask ptask)
{
    assert(list != NULL);

    ntg_fwd_list_push_back((ntg_fwd_list*)list, &ptask, sizeof(struct ntg_ptask));
}

void ntg_ptask_list_pop_front(ntg_ptask_list* list)
{
    assert(list != NULL);

    ntg_fwd_list_pop_front((ntg_fwd_list*)list);
}
