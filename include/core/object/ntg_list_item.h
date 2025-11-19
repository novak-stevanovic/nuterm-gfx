// #ifndef _NTG_LIST_ITEM_H_
// #define _NTG_LIST_ITEM_H_
//
// #include "core/object/ntg_object.h"
//
// typedef struct ntg_list_item ntg_list_item;
//
// struct ntg_list_item
// {
//     ntg_object __base;
//
//     ntg_object* __object;
//     ntg_orientation __list_orientation;
//     size_t __size;
// };
//
// void __ntg_list_item_init__(
//         ntg_list_item* item,
//         ntg_object* object,
//         ntg_orientation list_orientation,
//         size_t size);
// void __ntg_list_item_deinit__(ntg_list_item* item);
//
// size_t ntg_list_item_get_size(const ntg_list_item* item);
//
// const ntg_object* ntg_list_item_get_object(const ntg_list_item* item);
// ntg_object* ntg_list_item_get_object_(ntg_list_item* item);
//
// struct ntg_measure_output __ntg_list_item_measure_fn(
//         const ntg_drawable* drawable,
//         ntg_orientation orientation,
//         size_t for_size,
//         const ntg_measure_context* context);
//
// void __ntg_list_item_constrain_fn(
//         const ntg_drawable* drawable,
//         ntg_orientation orientation,
//         size_t size,
//         struct ntg_measure_output measure_output,
//         const ntg_constrain_context* context,
//         ntg_constrain_output* output);
//
// void __ntg_list_item_arrange_fn(
//         const ntg_drawable* drawable,
//         struct ntg_xy size,
//         const ntg_arrange_context* context,
//         ntg_arrange_output* output);
//
// #endif // _NTG_LIST_ITEM_H_
