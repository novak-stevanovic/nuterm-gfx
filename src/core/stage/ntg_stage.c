#include "ntg.h"
#include <assert.h>

static void init_default(ntg_stage* stage);

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
    ntg_entity_raise_event_((ntg_entity*)stage, NTG_EVENT_STAGE_SCNCHNG, &data);
}

bool ntg_stage_feed_event(ntg_stage* stage, struct ntg_event event)
{
    assert(stage != NULL);

    return stage->__process_fn(stage, event);
}

bool ntg_stage_dispatch_def(ntg_stage* stage, struct ntg_event event)
{
    if(stage->_scene)
    {
        if(event.type == NTG_EVENT_LOOP_KEY)
        {
            return ntg_scene_feed_event(stage->_scene, event);
        }
        else if(event.type == NTG_EVENT_LOOP_MOUSE)
        {
            return ntg_scene_feed_event(stage->_scene, event);
        }
        else return false;
    }
    else return false;
}

void ntg_stage_set_process_fn(ntg_stage* stage, ntg_stage_process_fn fn)
{
    assert(stage != NULL);

    stage->__process_fn = ntg_stage_dispatch_def;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_stage_init(ntg_stage* stage, ntg_stage_compose_fn compose_fn)
{
    assert(stage != NULL);
    assert(compose_fn != NULL);

    init_default(stage);

    ntg_stage_drawing_init(&stage->_drawing);
    stage->__compose_fn = compose_fn;
}

void ntg_stage_deinit(ntg_stage* stage)
{
    if(stage->_loop)
    {
        ntg_loop_set_stage(stage->_loop, NULL);
    }

    if(stage->_scene)
    {
        ntg_stage_set_scene(stage, NULL);
    }

    ntg_stage_drawing_deinit(&stage->_drawing);

    init_default(stage);
}

void _ntg_stage_set_loop(ntg_stage* stage, ntg_loop* loop)
{
    assert(stage != NULL);

    stage->_loop = loop;
}

static void init_default(ntg_stage* stage)
{
    stage->_scene = NULL;
    stage->__compose_fn = NULL;
    stage->_drawing = (struct ntg_stage_drawing) {0};
    stage->__process_fn = ntg_stage_dispatch_def;
    stage->_loop = NULL;
    stage->data = NULL;
}
