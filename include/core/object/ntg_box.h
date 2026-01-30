// #ifndef NTG_BOX_H
// #define NTG_BOX_H
// 
// #include "shared/ntg_shared.h"
// #include "core/object/ntg_widget.h"
// 
// /* -------------------------------------------------------------------------- */
// /* PUBLIC DEFINITIONS */
// /* -------------------------------------------------------------------------- */
// 
// struct ntg_box_opts
// {
//     ntg_orient orient;
//     ntg_align palign;
//     ntg_align salign;
//     size_t spacing;
// };
// 
// struct ntg_box_opts ntg_box_opts_def();
// 
// struct ntg_box
// {
//     ntg_widget __base;
// 
//     struct ntg_box_opts _opts;
//     ntg_widget_vec _children;
// };
// 
// /* -------------------------------------------------------------------------- */
// /* PUBLIC API */
// /* -------------------------------------------------------------------------- */
// 
// void ntg_box_init(ntg_box* box);
// void ntg_box_deinit(ntg_box* box);
// 
// struct ntg_box_opts ntg_box_get_opts(const ntg_box* box);
// void ntg_box_set_opts(ntg_box* box, struct ntg_box_opts opts);
// 
// void ntg_box_add_child(ntg_box* box, ntg_widget* child);
// void ntg_box_rm_child(ntg_box* box, ntg_widget* child);
// 
// /* -------------------------------------------------------------------------- */
// /* INTERNAL/PROTECTED */
// /* -------------------------------------------------------------------------- */
// 
// struct ntg_object_measure _ntg_box_measure_fn(
//         const ntg_widget* _box,
//         void* _ldata,
//         ntg_orient orient,
//         bool constrained,
//         sarena* arena);
// 
// void _ntg_box_constrain_fn(
//         const ntg_widget* _box,
//         void* _ldata,
//         ntg_orient orient,
//         ntg_widget_size_map* out_size_map,
//         sarena* arena);
// 
// void _ntg_box_arrange_fn(
//         const ntg_widget* _box,
//         void* _ldata,
//         ntg_widget_xy_map* out_pos_map,
//         sarena* arena);
// 
// #endif // NTG_BOX_H
