#ifndef NTG_STAGE_H
#define NTG_STAGE_H

#include "shared/ntg_shared.h"
#include "core/stage/ntg_stage_drawing.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

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

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes an empty stage and its rendered-cell drawing.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `stage` is `NULL`.
 * - `NTG_ERR_UNEXPECTED`: stage-drawing initialization fails. */
NTG_API void
ntg_stage_init(ntg_stage* stage, int* out_status);

/* ------------------------------------------------------ */

/* Detaches the stage from its loop and scene, releases drawing storage, and
 * clears the stage. Passing `NULL` has no effect. */
NTG_API void
ntg_stage_deinit(ntg_stage* stage);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_stage_deinit`, intended for cleanup
 * callbacks. */
NTG_API void
ntg_stage_deinit_v(void* _stage);

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

/* Marks the stage for recomposition. A `NULL` stage is ignored. */
NTG_API void
ntg_stage_mark_dirty(ntg_stage* stage);

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

/* Replaces the stage scene and keeps both sides of the relationship
 * synchronized. A scene already owned by another stage is first detached from
 * that stage; `NULL` clears the scene.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `stage` is `NULL`. */
NTG_API void
ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene, int* out_status);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

/* Default stage key handler that forwards the event to the current scene.
 *
 * RETURN VALUE:
 * `true` when the scene handles the event; otherwise `false`. */
NTG_API bool
ntg_stage_dispatch_key(ntg_stage* stage, struct nt_key_event key);

/* ------------------------------------------------------ */

/* Default stage mouse handler that forwards the event to the current scene.
 *
 * RETURN VALUE:
 * `true` when the scene handles the event; otherwise `false`. */
NTG_API bool
ntg_stage_dispatch_mouse(ntg_stage* stage, struct nt_mouse_event mouse);

/* ------------------------------------------------------ */

/* Invokes the stage key hook when present.
 *
 * RETURN VALUE:
 * `true` when the hook handles the event; otherwise `false`. */
NTG_API bool
ntg_stage_feed_key(ntg_stage* stage, struct nt_key_event key);

/* ------------------------------------------------------ */

/* Invokes the stage mouse hook when present.
 *
 * RETURN VALUE:
 * `true` when the hook handles the event; otherwise `false`. */
NTG_API bool
ntg_stage_feed_mouse(ntg_stage* stage, struct nt_mouse_event mouse);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* Clears the stage dirty flag after successful composition. */
void _ntg_stage_clean(ntg_stage* stage);

/* Assigns the owning loop pointer when it changes and marks the stage dirty
 * when attaching to a non-`NULL` loop. It does not update the opposite loop
 * link or invoke hooks. */
void _ntg_stage_set_loop(ntg_stage* stage, ntg_loop* loop);

/* Updates stage dimensions, propagates the size to the scene, invokes the size
 * hook, and marks the stage dirty. Drawing storage is resized later by
 * `_ntg_stage_compose`. */
void _ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size);
/* Lays out the current scene when needed and composites all scene layers into
 * the stage drawing. */
void _ntg_stage_compose(ntg_stage* stage, sarena* arena);

#endif // NTG_STAGE_H
