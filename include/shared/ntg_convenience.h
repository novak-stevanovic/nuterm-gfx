#ifndef NTG_CONVENIENCE
#define NTG_CONVENIENCE

#define ntg_obj(object_ptr) ((ntg_object*)(object_ptr))
#define ntg_bbox(border_box_ptr) ((ntg_border_box*)(border_box_ptr))
#define ntg_box(box_ptr) ((ntg_box*)(box_ptr))
#define ntg_pb(prog_bar_ptr) ((ntg_prog_bar*)(prog_bar_ptr))
#define ntg_cb(color_block_ptr) ((ntg_color_block*)(color_block_ptr))

#define ntg_scn(scene_ptr) ((ntg_scene*)(scene_ptr))

#define ntg_stg(stage_ptr) ((ntg_stage*)(stage_ptr))

#define ntg_rdr(renderer_ptr) ((ntg_renderer*)(renderer_ptr))
#define ntg_drnd(def_renderer_ptr) ((ntg_def_renderer*)(def_renderer_ptr))

#endif // NTG_CONVENIENCE
