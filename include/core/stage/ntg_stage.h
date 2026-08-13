#ifndef NTG_STAGE_H
#define NTG_STAGE_H

#include "shared/ntg_shared.h"
#include "core/stage/ntg_stage_drawing.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_stage_hooks
{
    void (*on_key_fn)(ntg_stage* stage, struct nt_key_event key);
    void (*on_mouse_fn)(ntg_stage* stage, struct nt_mouse_event mouse);

    void (*on_scene_chng_fn)(
            ntg_stage* stage,
            ntg_scene* old_scene,
            ntg_scene* new_scene);

    void (*on_size_chng_fn)(
            ntg_stage* stage,
            struct ntg_xy old_size,
            struct ntg_xy new_size);

    void (*on_loop_enter_fn)(ntg_stage* stage);
    void (*on_loop_leave_fn)(ntg_stage* stage);
};

struct ntg_stage
{
    const struct ntg_stage_vtable* __vtable;

    ntg_scene* _scene;

    struct ntg_xy _size;
    ntg_stage_drawing _drawing;

    bool _in_loop;

    bool _dirty;

    struct ntg_stage_hooks hooks;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_stage_init(ntg_stage* stage, int* out_status);

NTG_API void
ntg_stage_deinit(ntg_stage* stage);

NTG_API void
ntg_stage_deinit_void(void* _stage);

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

/* Can be used for custom loop */
NTG_API bool
ntg_stage_compose(ntg_stage* stage, sarena* arena);

NTG_API void
ntg_stage_mark_dirty(ntg_stage* stage);

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

NTG_API void
ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene, int* out_status);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_stage_feed_key(ntg_stage* stage, struct nt_key_event key);

NTG_API bool
ntg_stage_feed_mouse(ntg_stage* stage, struct nt_mouse_event mouse);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_stage_vtable
{
    bool (*handle_key_fn)(ntg_stage* stage, struct nt_key_event key);
    bool (*handle_mouse_fn)(ntg_stage* stage, struct nt_mouse_event mouse);
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_stage_init_override(
        ntg_stage* stage,
        const struct ntg_stage_vtable* vtable,
        int* out_status);

NTG_API bool
ntg_stage_dispatch_key_fn(ntg_stage* stage, struct nt_key_event key);

NTG_API bool
ntg_stage_dispatch_mouse_fn(ntg_stage* stage, struct nt_mouse_event mouse);

NTG_API extern const struct ntg_stage_vtable NTG_STAGE_VTABLE_DEFAULT;

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size);
void _ntg_stage_clean(ntg_stage* stage);
void _ntg_stage_enter_loop(ntg_stage* stage);
void _ntg_stage_leave_loop(ntg_stage* stage);

#endif // NTG_STAGE_H
