#ifndef NTG_RENDERER_H
#define NTG_RENDERER_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_renderer_hooks
{
    void (*on_render_fn)(
            ntg_renderer* renderer,
            const ntg_stage_drawing* drawing,
            sarena* arena);
};

struct ntg_renderer
{
    const struct ntg_renderer_vtable* __vtable;

    struct ntg_renderer_hooks hooks;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_renderer_init(ntg_renderer* renderer, int* out_status);

NTG_API void
ntg_renderer_deinit(ntg_renderer* renderer);

/* ------------------------------------------------------ */
/* RENDER */
/* ------------------------------------------------------ */

NTG_API void
ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_renderer_vtable
{
    void (*render_fn)(
            ntg_renderer* renderer,
            const ntg_stage_drawing* stage_drawing,
            sarena* arena);
};


/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_renderer_init_override(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable,
        int* out_status);

NTG_API void
ntg_renderer_render_fn(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

NTG_API extern const struct ntg_renderer_vtable NTG_RENDERER_VTABLE_DEFAULT;

#endif // NTG_RENDERER_H
