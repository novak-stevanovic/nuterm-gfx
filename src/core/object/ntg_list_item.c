// #include <assert.h>
//
// #include "core/object/ntg_list_item.h"
// #include "core/scene/shared/ntg_drawable_kit.h"
//
// void __ntg_list_item_init__(
//         ntg_list_item* item,
//         ntg_object* object,
//         ntg_orientation list_orientation,
//         size_t size)
// {
//     assert(item != NULL);
//     assert(object != NULL);
//     assert(size > 0);
//
//     __ntg_object_init__(
//             (ntg_object*)item,
//             NTG_OBJECT_DECORATOR,
//             __ntg_list_item_measure_fn,
//             __ntg_list_item_constrain_fn,
//             __ntg_list_item_arrange_fn,
//             NULL, NULL, NULL);
//
//     item->__object = object;
//     item->__list_orientation = list_orientation;
//     item->__size = size;
//
//     _ntg_object_add_child((ntg_object*)item, object);
// }
// void __ntg_list_item_deinit__(ntg_list_item* item)
// {
//     assert(item != NULL);
//
//     __ntg_object_deinit__((ntg_object*)item);
//
//     // TODO: fix for all objects
//     (*item) = (ntg_list_item) {0};
// }
//
// size_t ntg_list_item_get_size(const ntg_list_item* item)
// {
//     assert(item != NULL);
//
//     return item->__size;
// }
//
// const ntg_object* ntg_list_item_get_object(const ntg_list_item* item)
// {
//     assert(item != NULL);
//
//     return item->__object;
// }
// ntg_object* ntg_list_item_get_object_(ntg_list_item* item)
// {
//     assert(item != NULL);
//
//     return item->__object;
// }
//
// struct ntg_measure_output __ntg_list_item_measure_fn(
//         const ntg_drawable* drawable,
//         ntg_orientation orientation,
//         size_t for_size,
//         const ntg_measure_context* context)
// {
//     const ntg_list_item* item = (const ntg_list_item*)
//         ntg_drawable_user(drawable);
//
//     if(orientation == item->__list_orientation)
//     {
//         return (struct ntg_measure_output) {
//             .min_size = item->__size,
//             .natural_size = item->__size,
//             .max_size = item->__size,
//             .grow = 0
//         };
//     }
//     else
//     {
//         struct ntg_measure_data data = ntg_measure_context_get(
//                 context,
//                 ntg_object_get_drawable(item->__object));
//
//         return (struct ntg_measure_output) {
//             .min_size = data.min_size,
//             .natural_size = data.natural_size,
//             .max_size = data.max_size,
//             .grow = data.grow
//         };
//     }
// }
//
// void __ntg_list_item_constrain_fn(
//         const ntg_drawable* drawable,
//         ntg_orientation orientation,
//         size_t size,
//         struct ntg_measure_output measure_output,
//         const ntg_constrain_context* context,
//         ntg_constrain_output* output)
// {
//     const ntg_list_item* item = (const ntg_list_item*)
//         ntg_drawable_user(drawable);
//
//     struct ntg_constrain_result result = {
//         .size = size
//     };
//
//     ntg_constrain_output_set(output, ntg_object_get_drawable(item->__object), result);
// }
//
// void __ntg_list_item_arrange_fn(
//         const ntg_drawable* drawable,
//         struct ntg_xy size,
//         const ntg_arrange_context* context,
//         ntg_arrange_output* output)
// {
//     const ntg_list_item* item = (const ntg_list_item*)
//         ntg_drawable_user(drawable);
//
//     struct ntg_arrange_result result = {
//         .pos = ntg_xy(0, 0)
//     };
//
//     ntg_arrange_output_set(output, ntg_object_get_drawable(item->__object), result);
// }
