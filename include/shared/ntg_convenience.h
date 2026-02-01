#ifndef NTG_CONVENIENCE
#define NTG_CONVENIENCE

#define ntg_obj(object_ptr) ((ntg_object*)(object_ptr))
#define ntg_bbox(bbox_ptr) ((ntg_bbox*)(bbox_ptr))
#define ntg_box(box_ptr) ((ntg_box*)(box_ptr))
#define ntg_pb(pbar_ptr) ((ntg_pbar*)(pbar_ptr))
#define ntg_cb(cblock_ptr) ((ntg_cblock*)(cblock_ptr))

#define ntg_scn(scene_ptr) ((ntg_scene*)(scene_ptr))

#define ntg_stg(stage_ptr) ((ntg_stage*)(stage_ptr))

#define ntg_rdr(renderer_ptr) ((ntg_renderer*)(renderer_ptr))
#define ntg_drnd(def_renderer_ptr) ((ntg_def_renderer*)(def_renderer_ptr))

#endif // NTG_CONVENIENCE
