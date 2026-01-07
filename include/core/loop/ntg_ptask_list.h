#ifndef _NTG_PTASK_LIST_H_
#define _NTG_PTASK_LIST_H_

#include "shared/ntg_typedef.h"

struct ntg_ptask;

struct ntg_ptask_list_node
{
    struct ntg_ptask* data;
    struct ntg_ptask_list_node* next;
};

struct ntg_ptask_list
{
    size_t _count;
    struct ntg_ptask_list_node *_head, *_tail;
};

void ntg_ptask_list_init(ntg_ptask_list* list);
void ntg_ptask_list_deinit(ntg_ptask_list* list);

void ntg_ptask_list_push_front(ntg_ptask_list* list, struct ntg_ptask ptask);
void ntg_ptask_list_push_back(ntg_ptask_list* list, struct ntg_ptask ptask);
void ntg_ptask_list_pop_front(ntg_ptask_list* list);

#endif // _NTG_PTASK_LIST_H_
