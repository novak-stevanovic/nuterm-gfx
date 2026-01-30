// #include "ntg.h"
// #include <assert.h>
// #include <stdlib.h>
// #include "shared/ntg_shared_internal.h"
// 
// struct ntg_box_opts ntg_box_opts_def()
// {
//     return (struct ntg_box_opts) {
//         .orient = NTG_ORIENT_H,
//         .palign = NTG_ALIGN_1,
//         .salign = NTG_ALIGN_1,
//         .spacing = 0
//     };
// }
// 
// static inline size_t calculate_total_spacing(size_t spacing, size_t child_count);
// 
// static void on_rm_child_fn(ntg_widget* _box, ntg_widget* child);
// 
// /* -------------------------------------------------------------------------- */
// /* PUBLIC API */
// /* -------------------------------------------------------------------------- */
// 
// void ntg_box_init(ntg_box* box)
// {
//     assert(box != NULL);
// 
//     struct ntg_widget_layout_ops layout_ops = {
//         .measure_fn = _ntg_box_measure_fn,
//         .constrain_fn = _ntg_box_constrain_fn,
//         .arrange_fn = _ntg_box_arrange_fn,
//         .draw_fn = NULL,
//         .init_fn = NULL,
//         .deinit_fn = NULL
//     };
// 
//     struct ntg_widget_hooks hooks = {
//         .on_rm_child_fn = on_rm_child_fn
//     };
// 
//     ntg_widget_init((ntg_widget*)box, layout_ops, hooks);
// }
// 
// struct ntg_box_opts ntg_box_get_opts(const ntg_box* box)
// {
//     assert(box != NULL);
// 
//     return box->_opts;
// }
// 
// void ntg_box_set_opts(ntg_box* box, struct ntg_box_opts opts)
// {
//     assert(box != NULL);
//     
//     box->_opts = opts;
// 
//     ntg_object_add_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);
// }
// 
// void ntg_box_add_child(ntg_box* box, ntg_widget* child)
// {
//     assert(box != NULL);
//     assert(child != NULL);
// 
//     ntg_widget_attach((ntg_widget*)box, child);
// 
//     ntg_widget_vec_pushb(&box->_children, child, NULL);
// 
//     ntg_object_add_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);
// }
// 
// void ntg_box_rm_child(ntg_box* box, ntg_widget* child)
// {
//     assert(box != NULL);
//     assert(child != NULL);
// 
//     ntg_widget_detach(child);
// 
//     ntg_widget_vec_rm(&box->_children, child, NULL);
// 
//     ntg_object_add_dirty((ntg_object*)box, NTG_OBJECT_DIRTY_FULL);
// }
// 
// /* -------------------------------------------------------------------------- */
// /* INTERNAL/PROTECTED */
// /* -------------------------------------------------------------------------- */
// 
// void ntg_box_deinit(ntg_box* box)
// {
//     box->_opts = ntg_box_opts_def();
//     ntg_widget_deinit((ntg_widget*)box);
// }
// 
// struct ntg_object_measure _ntg_box_measure_fn(
//         const ntg_widget* _box,
//         void* _ldata,
//         ntg_orient orient,
//         bool constrained,
//         sarena* arena)
// {
//     const ntg_box* box = (const ntg_box*)_box;
//     const ntg_widget_vec* children = &box->_children;
// 
//     if(children->size == 0) return (struct ntg_object_measure) {0};
// 
//     size_t min_size = 0, nat_size = 0, max_size = 0;
// 
//     size_t i;
//     struct ntg_object_measure it_measure;
//     const ntg_widget* it_child;
//     for(i = 0; i < children->size; i++)
//     {
//         it_child = children->data[i];
//         it_measure = ntg_widget_get_measure(it_child, orient);
// 
//         /* Make sure all are drawn */
//         it_measure.nat_size = _max2_size(it_measure.nat_size, 1);
//         it_measure.max_size = _max2_size(it_measure.max_size, 1);
// 
//         if(orient == box->_opts.orient)
//         {
//             min_size += it_measure.min_size;
//             nat_size += it_measure.nat_size;
//             max_size += it_measure.max_size;
//         }
//         else
//         {
//             min_size = _max2_size(min_size, it_measure.min_size);
//             nat_size = _max2_size(nat_size, it_measure.nat_size);
//             max_size = _max2_size(max_size, it_measure.max_size);
//         }
//     }
// 
//     size_t spacing = calculate_total_spacing(box->_opts.spacing, children->size);
// 
//      struct ntg_object_measure measure = {
//         .min_size = min_size,
//         .nat_size = nat_size + spacing,
//         .max_size = NTG_SIZE_MAX,
//         .grow = 1
//     };
// 
//      return measure;
// }
// 
// void _ntg_box_constrain_fn(
//         const ntg_widget* _box,
//         void* _ldata,
//         ntg_orient orient,
//         ntg_widget_size_map* out_size_map,
//         sarena* arena)
// {
//     const ntg_box* box = (const ntg_box*)_box;
//     const ntg_widget_vec* children = &box->_children;
//     size_t size = ntg_widget_get_cont_size_1d(_box, orient);
// 
//     if(children->size == 0) return;
// 
//     struct ntg_object_measure cont_size;
//     cont_size = ntg_widget_get_cont_measure(_box, orient);
//     size_t min_size = cont_size.min_size;
//     size_t nat_size = cont_size.nat_size;
// 
//     size_t extra_size;
//     size_t array_size = children->size * sizeof(size_t);
//     size_t* caps = (size_t*)sarena_malloc(arena, array_size);
//     assert(caps != NULL);
//     size_t* _sizes = (size_t*)sarena_malloc(arena, array_size);
//     size_t* grows = NULL;
// 
//     const ntg_widget* it_child;
//     struct ntg_object_measure it_measure;
//     size_t i;
//     if(orient == box->_opts.orient)
//     {
//         if(size >= nat_size) // redistribute extra, capped with max_size
//         {
//             grows = (size_t*)sarena_malloc(arena, array_size);
//             assert(grows != NULL);
//             extra_size = size - nat_size; // spacing is included in nat_size
// 
//             for(i = 0; i < children->size; i++)
//             {
//                 it_child = children->data[i];
//                 it_measure = ntg_widget_get_measure(it_child, orient);
// 
//                 caps[i] = it_measure.max_size;
//                 _sizes[i] = it_measure.nat_size;
//                 grows[i] = it_measure.grow;
//             }
//         }
//         else
//         {
//             if(size >= min_size) // redistribute extra, capped with nat_size
//             {
//                 size_t pref_spacing = calculate_total_spacing(box->_opts.spacing, children->size);
//                 extra_size = _ssub_size(size - min_size, pref_spacing);
// 
//                 for(i = 0; i < children->size; i++)
//                 {
//                     it_child = children->data[i];
//                     it_measure = ntg_widget_get_measure(it_child, orient);
// 
//                     caps[i] = it_measure.nat_size;
//                     _sizes[i] = it_measure.min_size;
//                 }
//             }
//             else // redistribute all, capped with min_size
//             {
//                 extra_size = size;
// 
//                 for(i = 0; i < children->size; i++)
//                 {
//                     it_child = children->data[i];
//                     it_measure = ntg_widget_get_measure(it_child, orient);
// 
//                     caps[i] = it_measure.min_size;
//                     _sizes[i] = 0;
//                 }
//             }
//         }
// 
//         ntg_sap_cap_round_robin(caps, grows, _sizes, extra_size, children->size, arena);
// 
//         for(i = 0; i < children->size; i++)
//         {
//             it_child = children->data[i];
// 
//             ntg_widget_size_map_set(out_size_map, it_child, _sizes[i]);
//         }
//     }
//     else
//     {
//         size_t it_size;
// 
//         for(i = 0; i < children->size; i++)
//         {
//             it_child = children->data[i];
//             it_measure = ntg_widget_get_measure(it_child, orient);
// 
//             it_size =_min2_size(size, 
//                     (it_measure.grow > 0 ?
//                      it_measure.max_size :
//                      it_measure.nat_size));
//             it_size = (it_size > 0) ? it_size : 1;
// 
//             ntg_widget_size_map_set(out_size_map, it_child, it_size);
//         }
//     }
// }
// 
// void _ntg_box_arrange_fn(
//         const ntg_widget* _box,
//         void* _ldata,
//         ntg_widget_xy_map* out_pos_map,
//         sarena* arena)
// {
//     const ntg_box* box = (const ntg_box*)_box;
//     const ntg_widget_vec* children = &box->_children;
//     struct ntg_xy size = ntg_widget_get_cont_size(_box);
// 
//     if(children->size == 0) return;
// 
//     /* Init */
//     ntg_orient orient = box->_opts.orient;
//     ntg_align prim_align = box->_opts.palign;
//     ntg_align sec_align = box->_opts.salign;
// 
//     size_t i;
//     const ntg_widget* it_child;
//     struct ntg_xy it_size;
//     struct ntg_oxy _it_size;
// 
//     /* Calculate children size */
//     struct ntg_oxy _size = ntg_oxy_from_xy(size, orient);
//     struct ntg_oxy _children_size = ntg_oxy(0, 0, orient);
//     for(i = 0; i < children->size; i++)
//     {
//         it_child = children->data[i];
// 
//         it_size = ntg_widget_get_size(it_child);
//         _it_size = ntg_oxy_from_xy(it_size, orient);
// 
//         _children_size.prim_val += _it_size.prim_val;
//         _children_size.sec_val = _max2_size(_children_size.sec_val, _it_size.sec_val);
//     }
// 
//     /* Calculate spacing */
//     size_t pref_spacing = calculate_total_spacing(box->_opts.spacing, children->size);
//     size_t total_spacing = _min2_size(pref_spacing, _size.prim_val - _children_size.prim_val);
// 
//     /* Distribute padding space */
//     size_t array_size = children->size * sizeof(size_t);
// 
//     size_t* spacing_caps = (size_t*)sarena_malloc(arena, array_size);
//     assert(spacing_caps != NULL);
//     for(i = 0; i < (children->size - 1); i++)
//         spacing_caps[i] = NTG_SIZE_MAX;
// 
//     size_t* _spacing_after = (size_t*)sarena_calloc(arena, array_size);
//     assert(_spacing_after != NULL);
// 
//     ntg_sap_cap_round_robin(spacing_caps, NULL, _spacing_after,
//             total_spacing, children->size - 1, arena);
// 
//     /* Calculate cont size */
//     struct ntg_oxy _cont_size = ntg_oxy(
//             _children_size.prim_val + total_spacing,
//             _children_size.sec_val, orient);
// 
//     /* Calculate base offset */
//     struct ntg_oxy _base_offset = ntg_oxy(0, 0, orient);
//     if(prim_align == NTG_ALIGN_1)
//         _base_offset.prim_val = 0;
//     else if(prim_align == NTG_ALIGN_2)
//         _base_offset.prim_val = (_size.prim_val - _cont_size.prim_val) / 2;
//     else
//         _base_offset.prim_val = _size.prim_val - _cont_size.prim_val;
// 
//     if(sec_align == NTG_ALIGN_1)
//         _base_offset.sec_val = 0;
//     else if(sec_align == NTG_ALIGN_2)
//         _base_offset.sec_val = (_size.sec_val - _cont_size.sec_val) / 2;
//     else
//         _base_offset.sec_val = (_size.sec_val - _cont_size.sec_val);
// 
//     struct ntg_oxy _it_extra_offset = ntg_oxy(0, 0, orient);
//     struct ntg_xy it_pos;
//     for(i = 0; i < children->size; i++)
//     {
//         it_child = children->data[i];
// 
//         it_size = ntg_widget_get_size(it_child);
//         _it_size = ntg_oxy_from_xy(it_size, orient);
// 
//         /* Calculate offset from secondary align */
//         if(sec_align == NTG_ALIGN_1)
//             _it_extra_offset.sec_val = 0;
//         else if(sec_align == NTG_ALIGN_2)
//             _it_extra_offset.sec_val = (_cont_size.sec_val - _it_size.sec_val) / 2;
//         else
//             _it_extra_offset.sec_val = (_cont_size.sec_val - _it_size.sec_val);
// 
//         /* Set pos of child */
//         it_pos = ntg_xy_add(
//                 ntg_xy_from_oxy(_base_offset),
//                 ntg_xy_from_oxy(_it_extra_offset));
// 
//         ntg_widget_xy_map_set(out_pos_map, it_child, it_pos);
// 
//         /* Calculate next primary axis pos */
//         _it_extra_offset.prim_val += (_it_size.prim_val + _spacing_after[i]);
//     }
// }
// 
// static inline size_t calculate_total_spacing(size_t spacing, size_t child_count)
// {
//     return (child_count > 0) ? ((child_count - 1) * spacing) : 0;
// }
// 
// static void on_rm_child_fn(ntg_widget* _box, ntg_widget* child)
// {
//     ntg_box* box = (ntg_box*)_box;
// 
//     ntg_box_rm_child(box, child);
// }
