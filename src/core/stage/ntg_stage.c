#include <assert.h>

#include "core/stage/ntg_stage.h"
#include "core/scene/ntg_scene.h"
#include "core/stage/shared/ntg_stage_drawing.h"

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size)
{
    assert(stage != NULL);

    ntg_stage_drawing_set_size(stage->_drawing, size);

    if(stage->_scene != NULL)
        ntg_scene_layout(stage->_scene, size);
    stage->__compose_fn(stage, size);
}

const ntg_stage_drawing* ntg_stage_get_drawing(const ntg_stage* stage)
{
    assert(stage != NULL);

    return ((const ntg_stage_drawing*)stage->_drawing);
}

ntg_scene* ntg_stage_get_scene(ntg_stage* stage)
{
    assert(stage != NULL);

    return stage->_scene;
}

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);

    stage->_scene = scene;
}

bool ntg_stage_feed_key_event(
        ntg_stage* stage,
        struct nt_key_event key_event,
        ntg_loop_ctx* loop_ctx)
{
    assert(stage != NULL);

    if(stage->_scene != NULL)
        return ntg_scene_feed_key_event(stage->_scene, key_event, loop_ctx);
    else
        return false;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_stage_init_(ntg_stage* stage, ntg_stage_compose_fn compose_fn)
{
    assert(stage != NULL);
    assert(compose_fn != NULL);

    stage->_scene = NULL;
    stage->_drawing = ntg_stage_drawing_new();
    stage->__compose_fn = compose_fn;
    stage->data = NULL;
}

void _ntg_stage_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_stage* stage = (ntg_stage*)entity;
    ntg_stage_drawing_destroy(stage->_drawing);

    stage->_scene = NULL;
    stage->_drawing = NULL;
    stage->__compose_fn = NULL;
    stage->data = NULL;

    _ntg_entity_deinit_fn(entity);
}

