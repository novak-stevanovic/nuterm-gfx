#include <assert.h>
#include <stdlib.h>

#include "core/ntg_scene_engine.h"
#include "base/ntg_cell.h"
#include "core/ntg_scene.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "core/ntg_scene_drawing.h"
#include "shared/ntg_log.h"

struct ntg_scene_engine
{
    ntg_scene_t* scene;
};

static void _nsize_all(ntg_object_t* curr_obj)
{
    if(curr_obj == NULL) return;

    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);

    ntg_object_t* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _nsize_all(it_obj);
    }

    ntg_object_calculate_nsize(curr_obj);
}

static void _constrain_all(ntg_object_t* curr_obj)
{
    if(curr_obj == NULL) return;

    ntg_object_constrain(curr_obj);
    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);

    ntg_object_t* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _constrain_all(it_obj);
    }
}

static void _measure_all(ntg_object_t* curr_obj)
{
    if(curr_obj == NULL) return;

    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);

    ntg_object_t* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _measure_all(it_obj);
    }
    ntg_object_measure(curr_obj);
}

static void _arrange_all(ntg_object_t* curr_obj)
{
    if(curr_obj == NULL) return;

    ntg_object_arrange(curr_obj);
    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);

    ntg_object_t* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _arrange_all(it_obj);
    }
}

static void _draw_all(ntg_object_t* curr_obj, ntg_scene_drawing_t* scene_drawing)
{
    if(curr_obj == NULL) return;

    const ntg_object_drawing_t* obj_drawing = ntg_object_get_drawing(curr_obj);
    size_t i, j;
    if(obj_drawing != NULL)
    {
        struct ntg_xy obj_drawing_size = ntg_object_drawing_get_vp_size(obj_drawing);
        struct ntg_xy scene_drawing_size = ntg_scene_drawing_get_size(scene_drawing);

        struct ntg_xy obj_pos = ntg_object_get_position_abs(curr_obj);
        const ntg_cell_t* it_obj_cell;
        struct ntg_rcell* it_scene_cell;
        struct ntg_xy it_obj_pos, it_scene_pos;
        for(i = 0; i < obj_drawing_size.y; i++)
        {
            for(j = 0; j < obj_drawing_size.x; j++)
            {
                it_obj_pos = ntg_xy(j, i);
                it_scene_pos = ntg_xy_add(it_obj_pos, obj_pos);

                //assert(it_scene_pos.isInBounds());
                it_obj_cell = ntg_object_drawing_vp_at(obj_drawing, it_obj_pos);
                it_scene_cell = ntg_scene_drawing_at_(scene_drawing, it_scene_pos);

                (*it_scene_cell) = ntg_cell_overwrite(*it_obj_cell, *it_scene_cell);
            }
        }
    }

    const ntg_object_vec_t* children = ntg_object_get_children(curr_obj);

    ntg_object_t* it_obj;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _draw_all(it_obj, scene_drawing);
    }
}

ntg_scene_engine_t* ntg_scene_engine_new(ntg_scene_t* scene)
{
    if(scene == NULL) return NULL;

    ntg_scene_engine_t* new = (ntg_scene_engine_t*)malloc
        (sizeof(struct ntg_scene_engine));

    if(new == NULL) return NULL;

    new->scene = scene;

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

    const ntg_scene_drawing_t* drawing = ntg_scene_get_drawing(engine->scene);
    struct ntg_xy size = ntg_scene_drawing_get_size(drawing);

    ntg_object_layout_root(root, size);

    _nsize_all(root);
    _constrain_all(root);
    _measure_all(root);
    _arrange_all(root);
    _draw_all(root, _ntg_scene_get_drawing(engine->scene));
}
