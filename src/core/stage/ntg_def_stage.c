#include "ntg.h"
#include <assert.h>

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

void ntg_def_stage_init(ntg_def_stage* stage, ntg_loop* loop)
{
    assert(stage != NULL);

    _ntg_stage_init((ntg_stage*)stage, loop, _ntg_def_stage_compose_fn);

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

static void draw_fn(ntg_object* object, void* _stage)
{
    ntg_stage* stage = (ntg_stage*)_stage;

    const ntg_object_drawing* drawing = ntg_object_get_drawing(object, NTG_OBJECT_SELF);
    struct ntg_xy size = ntg_object_get_size(object, NTG_OBJECT_SELF);
    struct ntg_xy pos = ntg_object_get_position_abs(object, NTG_OBJECT_SELF);

    ntg_object_drawing_place_(drawing, ntg_xy(0, 0), size, &stage->_drawing, pos);
}

NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(draw_tree, draw_fn);

void _ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size, sarena* arena)
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
            it_cell = ntg_stage_drawing_at_(&_stage->_drawing, ntg_xy(j, i));
            (*it_cell) = ntg_cell_default();
        }
    }

    if((_stage->_scene != NULL) && (_stage->_scene->_root != NULL))
    {
        ntg_object* root = ntg_object_get_group_root_(_stage->_scene->_root);
        if(root != NULL) draw_tree(root, stage);
    }

    stage->__detected_changes = false;
    stage->__old_size = size;
}

static void observe_fn(ntg_entity* entity, struct ntg_event event)
{
    ntg_def_stage* stage = (ntg_def_stage*)entity;

    if(event.type == NTG_EVENT_SCENE_DIFF)
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
