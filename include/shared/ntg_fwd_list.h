#ifndef _NTG_FWD_LIST_H_
#define _NTG_FWD_LIST_H_

#include <stddef.h>
#include "shared/ntg_typedef.h"

// DON'T CHANGE STRUCTS

struct ntg_fwd_list_node
{
    void* data;
    struct ntg_fwd_list_node* next;
};

struct ntg_fwd_list
{
    size_t _count;
    struct ntg_fwd_list_node *_head, *_tail;
};

void ntg_fwd_list_init(ntg_fwd_list* list);
void ntg_fwd_list_deinit(ntg_fwd_list* list);

void ntg_fwd_list_push_front(ntg_fwd_list* list, const void* data, size_t data_size);
void ntg_fwd_list_push_back(ntg_fwd_list* list, const void* data, size_t data_size);
void ntg_fwd_list_pop_front(ntg_fwd_list* list);

#endif // _NTG_LIST_H_
