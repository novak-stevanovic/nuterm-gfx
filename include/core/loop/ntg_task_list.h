#ifndef _NTG_TASK_LIST_H_
#define _NTG_TASK_LIST_H_

#include "shared/ntg_typedef.h"

struct ntg_task;

struct ntg_task_list_node
{
    struct ntg_task* data;
    struct ntg_task_list_node* next;
};

struct ntg_task_list
{
    size_t _count;
    struct ntg_task_list_node *_head, *_tail;
};

void ntg_task_list_init(ntg_task_list* list);
void ntg_task_list_deinit(ntg_task_list* list);

void ntg_task_list_push_front(ntg_task_list* list, struct ntg_task task);
void ntg_task_list_push_back(ntg_task_list* list, struct ntg_task task);
void ntg_task_list_pop_front(ntg_task_list* list);

#endif // _NTG_TASK_LIST_H_
