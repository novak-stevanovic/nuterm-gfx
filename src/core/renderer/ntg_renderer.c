#include "core/renderer/ntg_renderer.h"

#include <assert.h>

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing)
{
    assert(renderer != NULL);

    renderer->__render_fn(renderer, stage_drawing);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_renderer_init_(ntg_renderer* renderer, ntg_renderer_render_fn render_fn)
{
    assert(renderer != NULL);
    assert(render_fn != NULL);

    renderer->__render_fn = render_fn;
    renderer->data = NULL;
}

void _ntg_renderer_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_renderer* renderer = (ntg_renderer*)entity;
    renderer->__render_fn = NULL;
    renderer->data = NULL;
}
