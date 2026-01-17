#ifndef NTG_RENDERER_H
#define NTG_RENDERER_H

#include "core/entity/ntg_entity.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_renderer
{
    ntg_entity __base;
    ntg_renderer_render_fn __render_fn;
    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

/* If `stage_drawing` is NULL, render empty */
void ntg_renderer_render(ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        sarena* arena);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_renderer_init(ntg_renderer* renderer, ntg_renderer_render_fn render_fn);
void _ntg_renderer_deinit_fn(ntg_entity* entity);

#endif // NTG_RENDERER_H
