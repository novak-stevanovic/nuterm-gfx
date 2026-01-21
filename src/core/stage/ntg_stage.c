#include "ntg.h"
#include <assert.h>

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size, sarena* arena)
{
    assert(stage != NULL);

    struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
    ntg_stage_drawing_set_size(&stage->_drawing, size, size_cap);

    if(stage->_scene != NULL)
        ntg_scene_layout(stage->_scene, size, arena);

    stage->__compose_fn(stage, size, arena);
}

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);
    if(stage->_scene == scene) return;

    ntg_scene* old_scene = stage->_scene;

    if(old_scene)
    {
        _ntg_scene_set_stage(old_scene, NULL);
    }

    stage->_scene = scene;

    if(scene)
    {
        ntg_stage* old_stage = scene->_stage;

        if(old_stage)
        {
            ntg_stage_set_scene(old_stage, NULL);
        }

        _ntg_scene_set_stage(scene, stage);
    }

    struct ntg_event_stage_scnchng_data data = {
        .old = old_scene,
        .new = scene
    };
    ntg_entity_raise_event((ntg_entity*)stage, NULL,
            NTG_EVENT_STAGE_SCNCHNG, &data);
}

bool ntg_stage_feed_event(
        ntg_stage* stage,
        struct nt_event event,
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

void _ntg_stage_init(ntg_stage* stage, ntg_loop* loop,
        ntg_stage_compose_fn compose_fn)
{
    assert(stage != NULL);
    assert(compose_fn != NULL);
    assert(loop != NULL);

    stage->_loop = loop;
    stage->_scene = NULL;

    ntg_stage_drawing_init(&stage->_drawing);
    stage->__compose_fn = compose_fn;

    stage->__event_fn = NULL;
    stage->__event_mode = NTG_STAGE_EVENT_DISPATCH_FIRST;

    stage->data = NULL;
}

void _ntg_stage_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_stage* stage = (ntg_stage*)entity;

    if(stage->_scene)
    {
        ntg_stage_set_scene(stage, NULL);
    }

    ntg_stage_drawing_deinit(&stage->_drawing);

    stage->_scene = NULL;
    stage->__compose_fn = NULL;
    stage->data = NULL;
    stage->_loop = NULL;
}
