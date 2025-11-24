#include <assert.h>

#include "core/stage/ntg_def_stage.h"
#include "core/scene/ntg_drawable.h"
#include "core/scene/ntg_scene.h"
#include "core/scene/shared/_ntg_drawing.h"


void __ntg_def_stage_init__(ntg_def_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);
    assert(scene != NULL);

    __ntg_stage_init__((ntg_stage*)stage, scene, __ntg_def_stage_compose_fn);
}

void __ntg_def_stage_deinit__(ntg_def_stage* stage)
{
    assert(stage != NULL);

    __ntg_stage_deinit__((ntg_stage*)stage);
}

static void __draw_fn(ntg_drawable* drawable, void* _stage)
{
    ntg_stage* stage = (ntg_stage*)_stage;

    struct ntg_scene_node node = ntg_scene_get_node(stage->__scene, drawable);

    ntg_drawing_place_(
            node.drawing,
            ntg_xy(0, 0),
            node.size,
            stage->__drawing,
            node.position);
}

void __ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size)
{
    assert(_stage != NULL);

    ntg_drawable* root = ntg_scene_get_root(_stage->__scene);

    ntg_drawable_tree_perform(
            root,
            NTG_DRAWABLE_PERFORM_TOP_DOWN,
            __draw_fn,
            _stage);
}
