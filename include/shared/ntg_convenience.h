#ifndef NTG_CONVENIENCE_H
#define NTG_CONVENIENCE_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC - MACROS */
/* ========================================================================== */

#define ntg_obj(object_ptr) ((ntg_object*)(object_ptr))
#define ntg_mp(main_panel_ptr) ((ntg_main_panel*)(main_panel_ptr))
#define ntg_box(box_ptr) ((ntg_box*)(box_ptr))
#define ntg_pb(prog_bar_ptr) ((ntg_prog_bar*)(prog_bar_ptr))
#define ntg_cb(color_block_ptr) ((ntg_color_block*)(color_block_ptr))
#define ntg_txt(text_ptr) ((ntg_text*)(text_ptr))

#define ntg_scn(scene_ptr) ((ntg_scene*)(scene_ptr))

#define ntg_stg(stage_ptr) ((ntg_stage*)(stage_ptr))

#define ntg_rnd(renderer_ptr) ((ntg_renderer*)(renderer_ptr))

#define NTG_CLEANUP_GEN_FN(fn_name, callee_fn)                                 \
static void fn_name(void* data)                                                \
{                                                                              \
    callee_fn(data);                                                           \
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Allocates an empty cleanup batch that stores cleanup entries in registration
 * order.
 *
 * RETURN VALUE:
 * A new batch, or `NULL` if the batch structure cannot be allocated. */
NTG_API ntg_cleanup_batch*
ntg_cleanup_batch_new();
/* For each entry in registration order, calls `deinit_fn` and then `free_fn`
 * when present; then releases the batch. Passing `NULL` has no effect. */
NTG_API void
ntg_cleanup_batch_finish(ntg_cleanup_batch* batch);

/* Adds one cleanup entry. `data`, `deinit_fn`, and `free_fn` may each be
 * `NULL`.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `batch` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: the cleanup-entry vector cannot grow.
 * - `NTG_ERR_UNEXPECTED`: the vector operation fails unexpectedly. */
NTG_API void
ntg_cleanup_batch_add(
        ntg_cleanup_batch* batch,
        void* data,
        void (*deinit_fn)(void* data),
        void (*free_fn)(void* data),
        int* out_status);

#endif // NTG_CONVENIENCE_H
