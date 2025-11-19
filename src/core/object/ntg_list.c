// #include "core/object/ntg_list.h"
// #include <assert.h>
//
// void __ntg_list_init__(
//         ntg_list* list,
//         ntg_orientation orientation,
//         size_t item_size,
//         ntg_process_key_fn process_key_fn,
//         ntg_on_focus_fn on_focus_fn)
// {
//     assert(list != NULL);
//     assert(item_size > 0);
//
//     __ntg_object_init__(
//             (ntg_object*)list,
//             NTG_OBJECT_WIDGET,
//             __ntg_list_measure_fn,
//             __ntg_list_constrain_fn,
//             __ntg_list_arrange_fn,
//             NULL,
//             process_key_fn,
//             on_focus_fn);
//
//     list->__item_size = item_size;
//     list->__items = ntg_object_vec_new();
// }
//
// void __ntg_list_deinit__(ntg_list* list)
// {
//     assert(list != NULL);
//
//     __ntg_object_deinit__((ntg_object*)list);
// }
//
// void ntg_list_add_item(ntg_list* list, ntg_object* child)
// {
//     assert(list != NULL);
//
//     assert(!ntg_object_vec_contains(list->__items, child));
//
//     ntg_object_vec_append(list->__items, child);
// }
//
// void ntg_list_remove_item(ntg_list* list, ntg_object* child)
// {
//     assert(list != NULL);
//
//     assert(ntg_object_vec_contains(list->__items, child));
//
//     ntg_object_vec_remove(list->__items, child);
// }
//
// // const ntg_object_vec* ntg_list_get_showing_items(const ntg_list* list);
// // const ntg_object_vec* ntg_list_get_all_items(const ntg_list* list);
//
// size_t ntg_list_get_item_size(const ntg_list* list)
// {
//     assert(list != NULL);
//
//     return list->__item_size;
// }
