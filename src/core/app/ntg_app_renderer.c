#include "core/app/ntg_app_renderer.h"

#include <assert.h>

void __ntg_app_renderer_init__(ntg_app_renderer* renderer,
        ntg_app_render_fn render_fn)
{
    assert(renderer != NULL);
    assert(render_fn != NULL);

    renderer->__render_fn = render_fn;
}

void __ntg_app_renderer_deinit__(ntg_app_renderer* renderer)
{
    assert(renderer != NULL);

    renderer->__render_fn = NULL;
}

void ntg_app_renderer_render(
        ntg_app_renderer* renderer,
        const ntg_scene_drawing* scene_drawing,
        struct ntg_xy size)
{
    assert(renderer != NULL);

    renderer->__render_fn(renderer, scene_drawing, size);
}
