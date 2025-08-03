#include <assert.h>
#include "core/ntg_stage.h"
#include "base/ntg_event_types.h"
#include "core/ntg_scene.h"
#include "object/ntg_object.h"
#include "shared/ntg_xy.h"
#include "base/ntg_event_types.h"

static ntg_stage_status __process_key_fn_default(ntg_stage* stage,
        struct nt_key_event key_event);

/* -------------------------------------------------------------------------- */

void __ntg_stage_init__(ntg_stage* stage,
        ntg_stage_render_fn render_fn)
{
    assert(stage != NULL);

    stage->_active_scene = NULL;
    stage->__process_key_fn = __process_key_fn_default;
    stage->__render_fn = render_fn;
    stage->_size = NTG_XY_UNSET;

    __ntg_listenable_init__(&stage->__listenable);
}

void __ntg_stage_deinit__(ntg_stage* stage)
{
    assert(stage != NULL);

    stage->_active_scene = NULL;
    stage->__process_key_fn = NULL;
    stage->__render_fn = NULL;
    stage->_size = NTG_XY_UNSET;

    __ntg_listenable_deinit__(&stage->__listenable);
}

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);

    if(stage->_active_scene == scene) return;

    struct ntg_scene_change data = {
        .old = stage->_active_scene,
        .new = scene
    };

    stage->_active_scene = scene;

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_STAGE_SCENE_CHANGE, stage, &data);
    ntg_listenable_raise(&stage->__listenable, &e);
}

void ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size)
{
    assert(stage != NULL);

    if(ntg_xy_are_equal(stage->_size, size)) return;

    struct ntg_size_change data = {
        .old = stage->_size,
        .new = size
    };

    stage->_size = size;

    if(stage->_active_scene != NULL)
    {
        stage->_size = size;

        if(stage->_active_scene != NULL)
        {
            ntg_scene_set_size(stage->_active_scene, size);
        }
    }

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_STAGE_RESIZE, stage, &data);
    ntg_listenable_raise(&stage->__listenable, &e);
}

void ntg_stage_render(ntg_stage* stage, ntg_stage_render_mode render_mode)
{
    assert(stage != NULL);

    stage->__render_fn(stage, render_mode);

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_STAGE_RENDER, stage, NULL);
    ntg_listenable_raise(&stage->__listenable, &e);
}

ntg_stage_status ntg_stage_feed_key_event(ntg_stage* stage,
        struct nt_key_event key_event)
{
    assert(stage != NULL);

    return stage->__process_key_fn(stage, key_event);
}

void _ntg_stage_set_process_key_fn(ntg_stage* stage,
        ntg_stage_process_key_fn process_key_fn)
{
    assert(stage != NULL);
    assert(process_key_fn != NULL);

    stage->__process_key_fn = process_key_fn;
}

void ntg_stage_listen(ntg_stage* stage, struct ntg_event_sub sub)
{
    assert(stage != NULL);

    ntg_listenable_listen(&stage->__listenable, sub);
}

void ntg_stage_stop_listening(ntg_stage* stage, void* subscriber)
{
    assert(stage != NULL);

    ntg_listenable_stop_listening(&stage->__listenable, subscriber);
}

ntg_listenable* _ntg_stage_get_listenable(ntg_stage* stage)
{
    assert(stage != NULL);

    return &stage->__listenable;
}

/* -------------------------------------------------------------------------- */

static ntg_stage_status __process_key_fn_default(ntg_stage* stage,
        struct nt_key_event key_event)
{
    assert(stage != NULL);

    if(key_event.type == NT_KEY_EVENT_UTF32)
    {
        switch(key_event.utf32_data.codepoint)
        {
            case 'q':
                 return NTG_STAGE_QUIT;
        }
    }

    if(stage->_active_scene != NULL)
        ntg_scene_feed_key_event(stage->_active_scene, key_event);

    return NTG_STAGE_CONTINUE;
}
