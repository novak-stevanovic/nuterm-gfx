#ifndef NTG_RENDERER_H
#define NTG_RENDERER_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_renderer_vtable
{
    void (*render_fn)(ntg_renderer* renderer,
            const ntg_stage_drawing* stage_drawing,
            sarena* arena);
};

struct ntg_renderer_hooks
{
    void (*on_render_fn)(
            ntg_renderer* renderer,
            const ntg_stage_drawing* drawing,
            sarena* arena);
};

struct ntg_renderer
{
    struct ntg_renderer_vtable __vtable;
    void* data;

    struct ntg_renderer_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* RENDER */
/* -------------------------------------------------------------------------- */

/* Invokes the renderer virtual function and then the render hook. Only a `NULL`
 * renderer is ignored; `stage_drawing` and `arena` are forwarded unchanged. */
NTG_API void
ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

/* Initializes a renderer by copying a virtual table that provides a render
 * callback.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `renderer` is `NULL`.
 * - `NTG_ERR_BAD_VTABLE`: `vtable` or its `render_fn` is `NULL`. */
NTG_API void
ntg_renderer_init(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable,
        int* out_status);

/* -------------------------------------------------------------------------- */

/* Clears a renderer base. Passing `NULL` has no effect. */
NTG_API void
ntg_renderer_deinit(ntg_renderer* renderer);

#endif // NTG_RENDERER_H
