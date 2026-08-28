#ifndef NTG_CONVENIENCE_H
#define NTG_CONVENIENCE_H

#include "shared/ntg_shared.h"
#include <string.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* MACROS */
/* ========================================================================== */

#define ntg_obj(obj_ptr) ((ntg_object*)(obj_ptr))

#define ntg_wgt(obj_ptr) ((ntg_widget*)(obj_ptr))
#define ntg_wgt_vtbl(obj_ptr) ((struct ntg_widget_vtable*)((ntg_obj(obj_ptr))->ro.vtable))

#define ntg_pnl(pnl_ptr) ((ntg_panel*)(pnl_ptr))
#define ntg_pnl_vtbl(pnl_ptr) ((struct ntg_panel_vtable*)((ntg_obj(pnl_ptr))->ro.vtable))

#define ntg_box(box_ptr) ((ntg_box*)(box_ptr))
#define ntg_box_vtbl(box_ptr) ((struct ntg_box_vtable*)((ntg_obj(box_ptr))->ro.vtable))

#define ntg_pb(pb_ptr) ((ntg_prog_bar*)(pb_ptr))
#define ntg_pb_vtbl(pb_ptr) ((struct ntg_prog_bar_vtable*)((ntg_obj(pb_ptr))->ro.vtable))

#define ntg_cb(cb_ptr) ((ntg_clr_block*)(cb_ptr))
#define ntg_cb_vtbl(cb_ptr) ((struct ntg_clr_block_vtable*)((ntg_obj(cb_ptr))->ro.vtable))

#define ntg_txt(txt_ptr) ((ntg_text*)(txt_ptr))
#define ntg_txt_vtbl(txt_ptr) ((struct ntg_text_vtable*)((ntg_obj(txt_ptr))->ro.vtable))

#define ntg_lbl(lbl_ptr) ((ntg_label*)(lbl_ptr))
#define ntg_lbl_vtbl(lbl_ptr) ((struct ntg_label_vtable*)((ntg_obj(lbl_ptr))->ro.vtable))

#define ntg_btn(btn_ptr) ((ntg_button*)(btn_ptr))
#define ntg_btn_vtbl(btn_ptr) ((struct ntg_button_vtable*)((ntg_obj(btn_ptr))->ro.vtable))

#define ntg_scn(scn_ptr) ((ntg_scene*)(scn_ptr))
#define ntg_scn_vtbl(scn_ptr) ((struct ntg_scene_vtable*)((ntg_obj(scn_ptr))->ro.vtable))

#define ntg_stg(stg_ptr) ((ntg_stage*)(stg_ptr))
#define ntg_stg_vtbl(stg_ptr) ((struct ntg_stage_vtable*)((ntg_obj(stg_ptr))->ro.vtable))

#define ntg_rnd(rnd_ptr) ((ntg_renderer*)(rnd_ptr))
#define ntg_rnd_vtbl(rnd_ptr) ((struct ntg_renderer_vtable*)((ntg_obj(rnd_ptr))->ro.vtable))

#define ntg_db_rnd(db_rnd_ptr) ((ntg_db_renderer*)(db_rnd_ptr))

#define ntg_bs(bs_ptr) ((ntg_border_style*)(bs_ptr))
#define ntg_bs_vtbl(bs_ptr) ((struct ntg_border_style_vtable*)((ntg_obj(bs_ptr))->ro.vtable))

#define ntg_ap(ap_ptr) ((ntg_anchor_policy*)(ap_ptr))
#define ntg_ap_vtbl(ap_ptr) ((struct ntg_anchor_policy_vtable*)((ntg_obj(ap_ptr))->ro.vtable))

#define ntg_object_zero(obj_ptr) \
    memset((unsigned char*)(obj_ptr) + sizeof((obj_ptr)->_base), 0, \
            sizeof(*(obj_ptr)) - sizeof((obj_ptr)->_base))

#define ntg_garbage_add_obj(garbage_ptr, obj_ptr) \
    ntg_garbage_add((garbage_ptr), (obj_ptr), ntg_object_vdeinit_void, NULL)

#define ntg_garbage_add_obj_free(batch_ptr, obj_ptr) \
    ntg_garbage_add((batch_ptr), (obj_ptr), ntg_object_vdeinit_void, free)

/* ========================================================================== */
/* GARBAGE */
/* ========================================================================== */

#define NTG_GARBAGE_CAP_AUTO 1000

NTG_API ntg_garbage*
ntg_garbage_new(size_t cap);

NTG_API void
ntg_garbage_throw(ntg_garbage* garbage);

NTG_API void
ntg_garbage_destroy(ntg_garbage* garbage);

NTG_API int
ntg_garbage_add(
        ntg_garbage* garbage,
        void* data,
        void (*deinit_fn)(void* data),
        void (*free_fn)(void* data));

#endif // NTG_CONVENIENCE_H
