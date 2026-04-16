#ifndef NTG_STAGE_H
#define NTG_STAGE_H

#include "shared/ntg_shared.h"
#include "core/stage/ntg_stage_drawing.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

// TODO
struct ntg_stage_hooks
{
    bool (*on_key_fn)(ntg_stage* stage, struct nt_key_event key);
    bool (*on_mouse_fn)(ntg_stage* stage, struct nt_mouse_event mouse);

    void (*on_scene_chng_fn)(
            ntg_stage* stage,
            ntg_scene* old_scene,
            ntg_scene* new_scene);

    void (*on_size_chng_fn)(
            ntg_stage* stage,
            struct ntg_xy old_size,
            struct ntg_xy new_size);

    void (*on_loop_chng_fn)(
            ntg_stage* stage,
            ntg_loop* old_loop,
            ntg_loop* new_loop);
};

struct ntg_stage
{
    ntg_scene* _scene;

    struct ntg_xy _size;
    ntg_stage_drawing _drawing;

    ntg_loop* _loop;

    bool _dirty;

    struct ntg_stage_hooks hooks;

    void* data;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

void ntg_stage_init(ntg_stage* stage);
void ntg_stage_deinit(ntg_stage* stage);
void ntg_stage_deinit_(void* _stage);

/* -------------------------------------------------------------------------- */
/* GENERAL */
/* -------------------------------------------------------------------------- */

void ntg_stage_mark_dirty(ntg_stage* stage);
void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

// Default implementations of `on_key_fn` and `on_mouse_fn`. Dispatches to scene
bool ntg_stage_dispatch_key(ntg_stage* stage, struct nt_key_event key);
bool ntg_stage_dispatch_mouse(ntg_stage* stage, struct nt_mouse_event mouse);

bool ntg_stage_feed_key(ntg_stage* stage, struct nt_key_event key);
bool ntg_stage_feed_mouse(ntg_stage* stage, struct nt_mouse_event mouse);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

void _ntg_stage_clean(ntg_stage* stage);

void _ntg_stage_set_loop(ntg_stage* stage, ntg_loop* loop);

// Called by ntg_loop
void _ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size);
void _ntg_stage_compose(ntg_stage* stage, sarena* arena);

#endif // NTG_STAGE_H
