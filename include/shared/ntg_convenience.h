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

#define ntg_mp(mp_ptr) ((ntg_main_panel*)(mp_ptr))
#define ntg_mp_vtbl(mp_ptr) ((struct ntg_main_panel_vtable*)((ntg_obj(mp_ptr))->ro.vtable))

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

#define NTG_CLEANUP_GEN_FN(fn_name, callee_fn)                                 \
static void fn_name(void* data)                                                \
{                                                                              \
    callee_fn(data);                                                           \
}

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API ntg_cleanup_batch*
ntg_cleanup_batch_new(void);

NTG_API int
ntg_cleanup_batch_finish(ntg_cleanup_batch* batch);

NTG_API int
ntg_cleanup_batch_add(
        ntg_cleanup_batch* batch,
        void* data,
        void (*deinit_fn)(void* data),
        void (*free_fn)(void* data));

#endif // NTG_CONVENIENCE_H
