#ifndef NTG_DEF_RENDERER_H
#define NTG_DEF_RENDERER_H

#include "shared/ntg_shared.h"
#include "core/renderer/ntg_renderer.h"
#include "core/stage/ntg_stage_drawing.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_def_renderer
{
    ntg_renderer __base;

    ntg_stage_drawing __backbuff;

    struct ntg_xy __old_size;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_def_renderer_init(ntg_def_renderer* renderer);
void ntg_def_renderer_deinit(ntg_def_renderer* renderer);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

#endif // NTG_DEF_RENDERER_H
