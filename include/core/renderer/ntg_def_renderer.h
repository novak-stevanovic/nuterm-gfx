#ifndef NTG_DEF_RENDERER_H
#define NTG_DEF_RENDERER_H

#include "shared/ntg_shared.h"
#include "core/renderer/ntg_renderer.h"
#include "core/stage/ntg_stage_drawing.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_def_renderer
{
    ntg_renderer __base;

    ntg_stage_drawing __backbuff;

    struct ntg_xy __old_size;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */


NTG_API void
ntg_def_renderer_init(ntg_def_renderer* renderer, int* out_status);

/* ------------------------------------------------------ */


NTG_API void
ntg_def_renderer_deinit(ntg_def_renderer* renderer);

/* ------------------------------------------------------ */


NTG_API void
ntg_def_renderer_deinit_(void* _renderer);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */


void _ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

NTG_API extern const struct ntg_renderer_vtable NTG_DEF_RENDERER_VTABLE;

#endif // NTG_DEF_RENDERER_H
