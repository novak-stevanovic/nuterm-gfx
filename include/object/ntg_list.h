#ifndef _NTG_LIST_H_
#define _NTG_LIST_H_

#include "object/ntg_box.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_vec.h"

typedef struct ntg_item_list ntg_item_list;

void __ntg_item_list_init__(ntg_item_list* list, ntg_orientation orientation);
void __ntg_item_list_deinit__(ntg_item_list* list);

void ntg_item_list_add_item(ntg_item_list* list, ntg_object* child);
void ntg_item_list_remove_item(ntg_item_list* list, ntg_object* child);
const ntg_object_vec* ntg_item_list_get_showing_items(const ntg_item_list* list);
const ntg_object_vec* ntg_item_list_get_all_items(const ntg_item_list* list);

void ntg_item_list_set_pref_item_size(ntg_item_list* list, size_t size);
size_t ntg_item_list_get_pref_item_size(const ntg_item_list* list);

void ntg_item_list_scroll(ntg_item_list* list, ssize_t scroll);

struct ntg_item_list
{
    ntg_box __box;

    ntg_object_vec __items;
    size_t __pref_item_size;
    ntg_orientation __orientation;
};

#endif // _NTG_LIST_H_
