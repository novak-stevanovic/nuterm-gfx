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

NTG_API void
ntg_renderer_vdeinit(ntg_renderer* renderer);

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

    void (*attach_loop_fn)(ntg_renderer* renderer);
    void (*detach_loop_fn)(ntg_renderer* renderer);

    void (*deinit_fn)(ntg_renderer* renderer);
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_renderer_init_inherit(
        ntg_renderer* renderer,
        const struct ntg_renderer_vtable* vtable,
        int* out_status);

NTG_API void
ntg_renderer_deinit(ntg_renderer* renderer);

#endif // NTG_RENDERER_H
