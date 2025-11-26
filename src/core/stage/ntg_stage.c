#include <assert.h>

#include "core/stage/ntg_stage.h"
#include "core/scene/ntg_scene.h"
#include "core/stage/shared/ntg_stage_drawing.h"

void __ntg_stage_init__(
        ntg_stage* stage,
        ntg_scene* scene,
        ntg_stage_compose_fn compose_fn,
        void* data)
{
    assert(stage != NULL);
    assert(scene != NULL);
    assert(compose_fn != NULL);

    stage->__scene = scene;
    stage->__drawing = ntg_stage_drawing_new();
    stage->__compose_fn = compose_fn;
    stage->__size = ntg_xy(0, 0);
    stage->__data = data;
}

void __ntg_stage_deinit__(ntg_stage* stage)
{
    assert(stage != NULL);

    ntg_stage_drawing_destroy(stage->__drawing);
    (*stage) = (ntg_stage) {0};
}

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size)
{
    assert(stage != NULL);

    ntg_scene_layout(stage->__scene, size);

    stage->__size = size;
    ntg_stage_drawing_set_size(stage->__drawing, size);

    stage->__compose_fn(stage, size);
}

const ntg_stage_drawing* ntg_stage_get_drawing(const ntg_stage* stage)
{
    assert(stage != NULL);

    return ((const ntg_stage_drawing*)stage->__drawing);
}

ntg_scene* ntg_stage_get_scene(ntg_stage* stage)
{
    assert(stage != NULL);

    return stage->__scene;
}

bool ntg_stage_feed_key_event(
        ntg_stage* stage,
        struct nt_key_event key_event,
        ntg_loop_context* loop_context)
{
    assert(stage != NULL);

    return ntg_scene_feed_key_event(stage->__scene, key_event, loop_context);
}
