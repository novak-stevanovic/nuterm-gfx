#include <assert.h>
#include <stdlib.h>

#include "ntg_core/ntg_scene_engine.h"
#include "ntg_base/ntg_cell.h"
#include "ntg_core/_ntg_scene.h"
#include "ntg_core/ntg_scene.h"
#include "ntg_object/ntg_object.h"

struct ntg_scene_engine
{
    ntg_scene_t* scene;
};

static void _constrain_all(ntg_object_t* curr_obj)
{
    ntg_object_constrain(curr_obj);
    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);
    size_t count = ntg_object_vec_size(children);

    ntg_object_t* it_obj;
    size_t i;
    for(i = 0; i < count; i++)
    {
        it_obj = ntg_object_vec_at(children, i);
        ntg_object_constrain(it_obj);
    }
}

static void _measure_all(ntg_object_t* curr_obj)
{
    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);
    size_t count = ntg_object_vec_size(children);

    ntg_object_t* it_obj;
    size_t i;
    for(i = 0; i < count; i++)
    {
        it_obj = ntg_object_vec_at(children, i);
        ntg_object_measure(it_obj);
    }
    ntg_object_measure(curr_obj);
}

static void _arrange_all(ntg_object_t* curr_obj)
{
    ntg_object_arrange(curr_obj);
    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);
    size_t count = ntg_object_vec_size(children);

    ntg_object_t* it_obj;
    size_t i;
    for(i = 0; i < count; i++)
    {
        it_obj = ntg_object_vec_at(children, i);
        ntg_object_arrange(it_obj);
    }
}

static void _draw_all(ntg_object_t* curr_obj, ntg_scene_content_t* scene_content)
{
    const ntg_object_content_t* obj_content = ntg_object_get_content(curr_obj);

    struct ntg_xy obj_content_size = ntg_object_content_get_size(obj_content);
    struct ntg_xy scene_content_size = ntg_scene_content_get_size(scene_content);

    struct ntg_xy obj_pos = ntg_object_get_position_abs(curr_obj);
    size_t i, j;
    const ntg_cell_t* it_obj_cell;
    struct ntg_cell_base* it_scene_cell;
    struct ntg_xy it_obj_pos, it_scene_pos;
    for(i = 0; i < obj_content_size.y; i++)
    {
        for(j = 0; j < obj_content_size.x; j++)
        {
            it_obj_pos = NTG_XY(j, i);
            it_scene_pos = NTG_XY_ADD(it_obj_pos, obj_pos);

            //assert(it_scene_pos.isInBounds());
            it_obj_cell = ntg_object_content_at(obj_content, it_obj_pos);
            it_scene_cell = ntg_scene_content_at_(scene_content, it_scene_pos);

            ntg_cell_overwrite(it_obj_cell, it_scene_cell);
        }
    }

    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);
    size_t count = ntg_object_vec_size(children);

    ntg_object_t* it_obj;
    for(i = 0; i < count; i++)
    {
        it_obj = ntg_object_vec_at(children, i);
        ntg_object_arrange(it_obj);
    }
}

ntg_scene_engine_t* ntg_scene_engine_new(ntg_scene_t* scene)
{
    if(scene == NULL) return NULL;

    ntg_scene_engine_t* new = (ntg_scene_engine_t*)malloc
        (sizeof(struct ntg_scene_engine));

    if(new == NULL) return NULL;

    return new;
}

void ntg_scene_engine_destroy(ntg_scene_engine_t* engine)
{
    if(engine == NULL) return;

    free(engine);
}

void ntg_scene_engine_layout(ntg_scene_engine_t* engine)
{
    ntg_object_t* root = ntg_scene_get_root(engine->scene);
    if(root == NULL) return;

    _constrain_all(root);
    _measure_all(root);
    _arrange_all(root);
    _draw_all(root, ntg_scene_get_content_(engine->scene));
}
