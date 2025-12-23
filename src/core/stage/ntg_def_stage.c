#include <assert.h>

#include "core/stage/ntg_def_stage.h"
#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/scene/ntg_scene.h"
#include "core/stage/shared/ntg_stage_drawing.h"

void _ntg_def_stage_init_(
        ntg_def_stage* stage,
        ntg_entity_group* group,
        ntg_entity_system* system)
{
    assert(stage != NULL);

    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_TYPE_DEF_STAGE,
        .deinit_fn = _ntg_def_stage_deinit_fn,
        .group = group,
        .system = system
    };

    struct ntg_stage_init_data stage_data = { .compose_fn = _ntg_def_stage_compose_fn };

    _ntg_stage_init_((ntg_stage*)stage, stage_data, entity_data);
}

void _ntg_def_stage_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    _ntg_stage_deinit_fn(entity);
}

static void __draw_fn(ntg_object* object, void* _stage)
{
    ntg_stage* stage = (ntg_stage*)_stage;

    struct ntg_scene_node node = ntg_scene_get_node(stage->_scene, object);

    ntg_object_drawing_place_(
            node.drawing,
            ntg_xy(0, 0),
            node.size,
            stage->_drawing,
            node.position);
}

void _ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size)
{
    assert(_stage != NULL);

    if((_stage->_scene != NULL) && (ntg_scene_get_root(_stage->_scene) != NULL))
    {
        ntg_object* root = ntg_scene_get_root(ntg_stage_get_scene(_stage));
        if(root != NULL)
        {
            ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN,
                    __draw_fn, _stage);
        }
    }
    else
    {
        size_t i, j;
        struct ntg_rcell* it_cell;
        for(i = 0; i < size.y; i++)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_stage_drawing_at_(_stage->_drawing, ntg_xy(j, i));
                (*it_cell) = ntg_rcell_default();
            }
        }
    }
}
