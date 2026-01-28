#ifndef NTG_DEF_RENDERER_H
#define NTG_DEF_RENDERER_H

#include "core/renderer/ntg_renderer.h"
#include "shared/ntg_xy.h"
#include "core/stage/shared/ntg_stage_drawing.h"

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

ntg_def_renderer* ntg_def_renderer_new(ntg_entity_system* system);
void ntg_def_renderer_init(ntg_def_renderer* renderer);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_def_renderer_deinit(ntg_def_renderer* renderer);

void _ntg_def_renderer_render_fn(
        ntg_renderer* _renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

#endif // NTG_DEF_RENDERER_H
