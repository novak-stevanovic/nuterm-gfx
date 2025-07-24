#include <assert.h>
#include "core/ntg_stage.h"
#include "core/ntg_scene.h"
#include "object/ntg_object.h"
#include "shared/ntg_xy.h"

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
}

void __ntg_stage_deinit__(ntg_stage* stage)
{
    assert(stage != NULL);

    stage->_active_scene = NULL;
    stage->__process_key_fn = NULL;
    stage->__render_fn = NULL;
}

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);

    stage->_active_scene = scene;
}

void ntg_stage_render(ntg_stage* stage, struct ntg_xy size,
        ntg_stage_render_mode render_mode)
{
    assert(stage != NULL);

    stage->__render_fn(stage, size, render_mode);
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
