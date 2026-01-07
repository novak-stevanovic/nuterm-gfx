#include "core/loop/ntg_task_list.h"
#include "shared/ntg_fwd_list.h"
#include "core/loop/_ntg_loop.h"
#include <assert.h>

void ntg_task_list_init(ntg_task_list* list)
{
    assert(list != NULL);

    ntg_fwd_list_init((ntg_fwd_list*)list);
}
void ntg_task_list_deinit(ntg_task_list* list)
{
    assert(list != NULL);

    ntg_fwd_list_deinit((ntg_fwd_list*)list);
}

void ntg_task_list_push_front(ntg_task_list* list, struct ntg_task task)
{
    assert(list != NULL);

    ntg_fwd_list_push_front((ntg_fwd_list*)list, &task, sizeof(struct ntg_task));
}

void ntg_task_list_push_back(ntg_task_list* list, struct ntg_task task)
{
    assert(list != NULL);

    ntg_fwd_list_push_back((ntg_fwd_list*)list, &task, sizeof(struct ntg_task));
}

void ntg_task_list_pop_front(ntg_task_list* list)
{
    assert(list != NULL);

    ntg_fwd_list_pop_front((ntg_fwd_list*)list);
}
