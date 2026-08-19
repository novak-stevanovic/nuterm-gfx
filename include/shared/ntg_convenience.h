#ifndef NTG_CONVENIENCE_H
#define NTG_CONVENIENCE_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* MACROS */
/* ========================================================================== */

#define ntg_obj(object_ptr) ((ntg_object*)(object_ptr))
#define ntg_mp(main_panel_ptr) ((ntg_main_panel*)(main_panel_ptr))
#define ntg_box(box_ptr) ((ntg_box*)(box_ptr))
#define ntg_pb(prog_bar_ptr) ((ntg_prog_bar*)(prog_bar_ptr))
#define ntg_cb(clr_block_ptr) ((ntg_clr_block*)(clr_block_ptr))
#define ntg_txt(text_ptr) ((ntg_text*)(text_ptr))
#define ntg_lbl(label_ptr) ((ntg_label*)(label_ptr))
#define ntg_btn(button_ptr) ((ntg_button*)(button_ptr))

#define ntg_scn(scene_ptr) ((ntg_scene*)(scene_ptr))

#define ntg_stg(stage_ptr) ((ntg_stage*)(stage_ptr))

#define ntg_rnd(renderer_ptr) ((ntg_renderer*)(renderer_ptr))

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
