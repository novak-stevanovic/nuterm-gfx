#include <assert.h>

#include "core/stage/ntg_stage.h"
#include "core/loop/ntg_loop.h"
#include "core/scene/ntg_scene.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "shared/ntg_log.h"

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

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);
    if(stage->_scene == scene) return;

    ntg_scene* old_scene = stage->_scene;
    ntg_stage* old_stage = scene->_stage;

    if(old_stage != NULL)
        old_stage->_scene = NULL;

    if(scene != NULL)
        _ntg_scene_set_stage(scene, stage);

    stage->_scene = scene;

    struct ntg_event_stage_scnchng_data data = {
        .old = old_scene,
        .new = scene
    };
    ntg_entity_raise_event((ntg_entity*)stage, NULL,
            NTG_EVENT_STAGE_SCNCHNG, &data);
}

bool ntg_stage_feed_event(
        ntg_stage* stage,
        struct ntg_loop_event event,
        ntg_loop_ctx* loop_ctx)
{
    assert(stage != NULL);

    bool consumed = false;
    if(stage->__event_mode == NTG_STAGE_EVENT_PROCESS_FIRST)
    {
        if(stage->__event_fn != NULL)
            consumed = stage->__event_fn(stage, event, loop_ctx);

        if(!consumed && (stage->_scene != NULL))
            ntg_scene_feed_event(stage->_scene, event, loop_ctx);
    }
    else // stage->__event_mode = NTG_STAGE_EVENT_DISPATCH_FIRST
    {
        if(stage->_scene != NULL)
            consumed = ntg_scene_feed_event(stage->_scene, event, loop_ctx);

        if(!consumed && (stage->__event_fn != NULL))
            consumed = stage->__event_fn(stage, event, loop_ctx);
    }

    return consumed;
}

void ntg_stage_set_event_fn(ntg_stage* stage, ntg_stage_event_fn fn)
{
    assert(stage != NULL);

    stage->__event_fn = fn;
}

void ntg_stage_set_event_mode(ntg_stage* stage, ntg_stage_event_mode mode)
{
    assert(stage != NULL);

    stage->__event_mode = mode;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_stage_init_(ntg_stage* stage, ntg_loop* loop,
        ntg_stage_compose_fn compose_fn)
{
    assert(stage != NULL);
    assert(compose_fn != NULL);
    assert(loop != NULL);

    stage->_loop = loop;
    stage->_scene = NULL;

    stage->_drawing = ntg_stage_drawing_new();
    stage->__compose_fn = compose_fn;

    stage->__event_fn = NULL;
    stage->__event_mode = NTG_STAGE_EVENT_DISPATCH_FIRST;

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
    stage->_loop = NULL;
}
