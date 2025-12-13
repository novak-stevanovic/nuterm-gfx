#include "core/renderer/ntg_renderer.h"

#include <assert.h>

void _ntg_renderer_init_(
        ntg_renderer* renderer,
        ntg_renderer_render_fn render_fn,
        void* data)
{
    assert(renderer != NULL);
    assert(render_fn != NULL);

    renderer->__render_fn = render_fn;
    renderer->_data = data;
}

void _ntg_renderer_deinit_(ntg_renderer* renderer)
{
    assert(renderer != NULL);

    renderer->__render_fn = NULL;
}

void ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size)
{
    assert(renderer != NULL);

    renderer->__render_fn(renderer, stage_drawing, size);
}
