#include <assert.h>
#include <stdlib.h>

#include "core/stage/ntg_def_stage.h"
#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/scene/ntg_scene.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "shared/ntg_log.h"

static void observe_fn(ntg_entity* entity, struct ntg_event event);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_stage* ntg_def_stage_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_DEF_STAGE,
        .deinit_fn = _ntg_def_stage_deinit_fn,
        .system = system
    };

    ntg_def_stage* new = (ntg_def_stage*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_def_stage_init_(ntg_def_stage* stage, ntg_loop* loop)
{
    assert(stage != NULL);

    _ntg_stage_init_((ntg_stage*)stage, loop, _ntg_def_stage_compose_fn);

    stage->__detected_changes = true;
    stage->__old_size = ntg_xy(0, 0);

    ntg_entity_observe((ntg_entity*)stage, (ntg_entity*)stage, observe_fn);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_stage_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_def_stage* stage = (ntg_def_stage*)entity;
    stage->__detected_changes = true;
    stage->__old_size = ntg_xy(0, 0);

    _ntg_stage_deinit_fn(entity);
}

static void __draw_fn(ntg_object* object, void* _stage)
{
    ntg_stage* stage = (ntg_stage*)_stage;

    struct ntg_scene_node node = ntg_scene_get_node(stage->_scene, object);

    ntg_object_drawing_place_(node.drawing, ntg_xy(0, 0),
            node.size, stage->_drawing, node.abs_pos);
}

void _ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size)
{
    assert(_stage != NULL);

    ntg_def_stage* stage = (ntg_def_stage*)_stage;

    if(ntg_xy_are_equal(size, stage->__old_size) && !stage->__detected_changes)
        return;

    size_t i, j;
    struct ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_stage_drawing_at_(_stage->_drawing, ntg_xy(j, i));
            (*it_cell) = ntg_cell_default();
        }
    }

    if((_stage->_scene != NULL) && (_stage->_scene->_root != NULL))
    {
        ntg_object* root = _stage->_scene->_root;
        if(root != NULL)
            ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN, __draw_fn, _stage);
    }

    stage->__detected_changes = false;
    stage->__old_size = size;
}

static void observe_fn(ntg_entity* entity, struct ntg_event event)
{
    ntg_def_stage* stage = (ntg_def_stage*)entity;

    if(event.type == NTG_EVENT_SCENE_CHANGE)
    {
        stage->__detected_changes = true;
    }
    else if(event.type == NTG_EVENT_STAGE_SCNCHNG)
    {
        struct ntg_event_stage_scnchng_data* data = event.data;

        if(data->old != NULL)
            ntg_entity_stop_observing((ntg_entity*)stage, (ntg_entity*)data->old, observe_fn);
        if(data->new != NULL)
            ntg_entity_observe((ntg_entity*)stage, (ntg_entity*)data->new, observe_fn);

        stage->__detected_changes = true;
    }
}
