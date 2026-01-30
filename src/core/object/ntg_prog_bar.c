// #include "ntg.h"
// #include <assert.h>
// #include <math.h>
// #include "shared/ntg_shared_internal.h"
// 
// struct ntg_prog_bar_opts ntg_prog_bar_opts_def()
// {
//     return (struct ntg_prog_bar_opts) {
//         .orient = NTG_ORIENT_H,
//         .complete_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(0, 255, 0))),
//         .uncomplete_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(255, 0, 0))),
//         .threshold_style = ntg_vcell_bg(nt_color_new_auto(nt_rgb_new(0, 255, 0))),
//     };
// }
// 
// void ntg_prog_bar_init(ntg_prog_bar* prog_bar)
// {
//     assert(prog_bar != NULL);
// 
//     struct ntg_widget_layout_ops widget_data = {
//         .measure_fn = _ntg_prog_bar_measure_fn,
//         .constrain_fn = NULL,
//         .arrange_fn = NULL,
//         .draw_fn = _ntg_prog_bar_draw_fn,
//         .init_fn = NULL,
//         .deinit_fn = NULL
//     };
// 
//     struct ntg_widget_hooks hooks = {0};
// 
//     ntg_widget_init((ntg_widget*)prog_bar, widget_data, hooks);
// 
//     prog_bar->__opts = ntg_prog_bar_opts_def();
// }
// 
// void ntg_prog_bar_set_percentage(ntg_prog_bar* prog_bar, double percentage)
// {
//     assert(prog_bar != NULL);
// 
//     prog_bar->__percentage = _min2_double(1.0, percentage);
// 
//     ntg_object_add_dirty((ntg_object*)prog_bar, NTG_OBJECT_DIRTY_DRAW);
// }
// 
// void ntg_prog_bar_deinit(ntg_prog_bar* prog_bar)
// {
//     prog_bar->__opts = ntg_prog_bar_opts_def();
// 
//     ntg_widget_deinit((ntg_widget*)prog_bar);
// }
// 
// struct ntg_object_measure _ntg_prog_bar_measure_fn(
//         const ntg_widget* _prog_bar,
//         void* _ldata,
//         ntg_orient orient,
//         bool constrained,
//         sarena* arena)
// {
//     const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;
// 
//     if(orient == prog_bar->__opts.orient)
//     {
//         return (struct ntg_object_measure) {
//             .min_size = 10,
//             .nat_size = 10,
//             .max_size = SIZE_MAX,
//             .grow = 1
//         };
//     }
//     else
//     {
//         return (struct ntg_object_measure) {
//             .min_size = 1,
//             .nat_size = 1,
//             .max_size = SIZE_MAX,
//             .grow = 1
//         };
//     }
// }
// 
// void _ntg_prog_bar_draw_fn(
//         const ntg_widget* _prog_bar,
//         void* _ldata,
//         ntg_tmp_object_drawing* out_drawing,
//         sarena* arena)
// {
//     const ntg_prog_bar* prog_bar = (const ntg_prog_bar*)_prog_bar;
//     struct ntg_xy size = ntg_widget_get_cont_size(_prog_bar);
// 
//     if(ntg_xy_is_zero(ntg_xy_size(size))) return;
// 
//     struct ntg_oxy _size = ntg_oxy_from_xy(size, prog_bar->__opts.orient);
// 
//     size_t complete_count = round(_size.prim_val * prog_bar->__percentage);
// 
//     size_t i, j;
//     struct ntg_oxy _it_xy;
//     struct ntg_xy it_xy;
//     struct ntg_vcell* it_cell;
//     for(i = 0; i < _size.prim_val; i++)
//     {
//         for(j = 0; j < _size.sec_val; j++)
//         {
//             _it_xy = ntg_oxy(i, j, prog_bar->__opts.orient);
//             it_xy = ntg_xy_from_oxy(_it_xy);
//             it_cell = ntg_tmp_object_drawing_get_(out_drawing, it_xy);
//             if(complete_count == _size.prim_val)
//                 (*it_cell) = prog_bar->__opts.complete_style;
//             else if(complete_count == 0)
//                 (*it_cell) = prog_bar->__opts.uncomplete_style;
//             else
//             {
//                 if(i < (complete_count - 1))
//                     (*it_cell) = prog_bar->__opts.complete_style;
//                 else if(i == (complete_count - 1))
//                     (*it_cell) = prog_bar->__opts.threshold_style;
//                 else
//                     (*it_cell) = prog_bar->__opts.uncomplete_style;
//             }
//         }
//     }
// }
