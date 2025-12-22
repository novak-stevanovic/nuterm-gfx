#include "core/renderer/ntg_renderer.h"

#include <assert.h>

void _ntg_renderer_init_(
        ntg_renderer* renderer,
        struct ntg_renderer_init_data renderer_data,
        struct ntg_entity_init_data entity_data)
{
    assert(renderer != NULL);
    assert(renderer_data.render_fn != NULL);

    if(entity_data.deinit_fn == NULL)
        entity_data.deinit_fn = _ntg_renderer_deinit_fn;
    assert(ntg_entity_instanceof(entity_data.type, &NTG_ENTITY_TYPE_RENDERER));
    _ntg_entity_init_((ntg_entity*)renderer, entity_data);

    renderer->__render_fn = renderer_data.render_fn;
    renderer->data = NULL;
}

void _ntg_renderer_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    _ntg_entity_deinit_fn(entity);

    ntg_renderer* renderer = (ntg_renderer*)entity;
    renderer->__render_fn = NULL;
    renderer->data = NULL;
}

void ntg_renderer_render(
        ntg_renderer* renderer,
        const ntg_stage_drawing* stage_drawing,
        struct ntg_xy size)
{
    assert(renderer != NULL);

    renderer->__render_fn(renderer, stage_drawing, size);
}
