// #ifndef _NTG_LIST_H_
// #define _NTG_LIST_H_
//
// #include "core/object/shared/ntg_object_vec.h"
// #include "core/object/ntg_box.h"
// #include "shared/ntg_xy.h"
//
// typedef struct ntg_list ntg_list;
// typedef struct ntg_object_vec ntg_object_vec;
//
// void __ntg_list_init__(
//         ntg_list* list,
//         ntg_orientation orientation,
//         size_t item_size,
//         ntg_process_key_fn process_key_fn,
//         ntg_on_focus_fn on_focus_fn);
// void __ntg_list_deinit__(ntg_list* list);
//
// void ntg_list_add_item(ntg_list* list, ntg_object* child);
// void ntg_list_remove_item(ntg_list* list, ntg_object* child);
// // const ntg_object_vec* ntg_list_get_showing_items(const ntg_list* list);
// // const ntg_object_vec* ntg_list_get_all_items(const ntg_list* list);
//
// size_t ntg_list_get_item_size(const ntg_list* list);
//
// void ntg_list_scroll(ntg_list* list, ssize_t scroll);
//
// struct ntg_list
// {
//     ntg_box __box;
//
//     ntg_object_vec* __items;
//     size_t __item_size;
//     ntg_orientation __orientation;
// };
//
// struct ntg_measure_output __ntg_list_measure_fn(
//         const ntg_drawable* drawable,
//         ntg_orientation orientation,
//         size_t for_size,
//         const ntg_measure_context* context);
//
// void __ntg_list_constrain_fn(
//         const ntg_drawable* drawable,
//         ntg_orientation orientation,
//         size_t size,
//         struct ntg_measure_output measure_output,
//         const ntg_constrain_context* context,
//         ntg_constrain_output* output);
//
// void __ntg_list_arrange_fn(
//         const ntg_drawable* drawable,
//         struct ntg_xy size,
//         const ntg_arrange_context* context,
//         ntg_arrange_output* output);
//
// #endif // _NTG_LIST_H_
