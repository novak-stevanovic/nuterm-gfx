#include <assert.h>

#include "core/stage/ntg_def_stage.h"
#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/scene/ntg_scene.h"

void _ntg_def_stage_init_(ntg_def_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);
    assert(scene != NULL);

    _ntg_stage_init_((ntg_stage*)stage, scene, _ntg_def_stage_compose_fn);
}

void _ntg_def_stage_deinit_(ntg_def_stage* stage)
{
    assert(stage != NULL);

    _ntg_stage_deinit_((ntg_stage*)stage);
}

static void __draw_fn(ntg_object* object, void* _stage)
{
    ntg_stage* stage = (ntg_stage*)_stage;

    struct ntg_scene_node node = ntg_scene_get_node(stage->__scene, object);

    ntg_object_drawing_place_(
            node.drawing,
            ntg_xy(0, 0),
            node.size,
            stage->__drawing,
            node.position);
}

void _ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size)
{
    assert(_stage != NULL);

    ntg_object* root = ntg_scene_get_root(ntg_stage_get_scene(_stage));

    if(root != NULL)
    {
        ntg_object_tree_perform(root,
                NTG_OBJECT_PERFORM_TOP_DOWN,
                __draw_fn,
                _stage);
    }
}
