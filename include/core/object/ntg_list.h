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
// struct ntg_measure_out __ntg_list_measure_fn(
//         const ntg_drawable* drawable,
//         ntg_orientation orientation,
//         size_t for_size,
//         const ntg_measure_ctx* ctx);
//
// void __ntg_list_constrain_fn(
//         const ntg_drawable* drawable,
//         ntg_orientation orientation,
//         size_t size,
//         struct ntg_measure_out measure_out,
//         const ntg_constrain_ctx* ctx,
//         ntg_constrain_out* out);
//
// void __ntg_list_arrange_fn(
//         const ntg_drawable* drawable,
//         struct ntg_xy size,
//         const ntg_arrange_ctx* ctx,
//         ntg_arrange_out* out);
//
// #endif // _NTG_LIST_H_
