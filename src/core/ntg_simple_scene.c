#include "core/ntg_simple_scene.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include <assert.h>
#include <stdlib.h>

static void __layout_fn(ntg_scene* _scene, struct ntg_xy size);

void __ntg_simple_scene_init__(ntg_simple_scene* scene,
        ntg_scene_process_key_fn process_key_fn)
{
    assert(scene != NULL);
    assert(process_key_fn != NULL);

    __ntg_scene_init__((ntg_scene*)scene, process_key_fn, __layout_fn);
}

void __ntg_simple_scene_deinit__(ntg_simple_scene* scene)
{
    assert(scene != NULL);

    __ntg_scene_deinit__((ntg_scene*)scene);
}

ntg_simple_scene* ntg_simple_scene_new(ntg_scene_process_key_fn process_key_fn)
{
    assert(process_key_fn != NULL);

    ntg_simple_scene* new = (ntg_simple_scene*)malloc(
            sizeof(struct ntg_simple_scene));

    assert(new != NULL);

    __ntg_simple_scene_init__(new, process_key_fn);

    return new;
}

void ntg_simple_scene_destroy(ntg_simple_scene* scene)
{
    assert(scene != NULL);

    __ntg_simple_scene_deinit__(scene);
    free(scene);
}

/* -------------------------------------------------------------------------- */

static void _nsize_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    const ntg_object_vec* children = curr_obj->_children;
    const ntg_aux_object_vec* aux_children = curr_obj->_aux_children;

    ntg_object* it_obj;
    size_t i;
    if(children != NULL)
    {
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            _nsize_all(it_obj, scene);
        }
    }

    for(i = 0; i < aux_children->_count; i++)
    {
        it_obj = aux_children->_data[i].object;
        _nsize_all(it_obj, scene);
    }

    ntg_object_calculate_nsize(curr_obj);
}

static void _constrain_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    ntg_object_constrain(curr_obj);
    const ntg_object_vec* children = curr_obj->_children;

    if(children)
    {
        ntg_object* it_obj;
        size_t i;
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            _constrain_all(it_obj, scene);
        }
    }
}

static void _constrain_aux(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    size_t i;
    ntg_object* it_obj;

    const ntg_aux_object_vec* aux_children = curr_obj->_aux_children;
    const ntg_object_vec* children = curr_obj->_children;

    if(curr_obj->_aux)
    {
        ntg_object_constrain(curr_obj);
    }
    else
    {
        for(i = 0; i < aux_children->_count; i++)
        {
            it_obj = aux_children->_data[i].object;
            _constrain_aux(it_obj, scene);
        }
    }

    if(children)
    {
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            _constrain_aux(it_obj, scene);
        }
    }
}

static void _measure_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    const ntg_object_vec* children = curr_obj->_children;

    if(children)
    {
        ntg_object* it_obj;
        size_t i;
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            _measure_all(it_obj, scene);
        }
    }

    ntg_object_measure(curr_obj);
}

static void _measure_aux(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    size_t i;
    ntg_object* it_obj;

    const ntg_aux_object_vec* aux_children = curr_obj->_aux_children;
    const ntg_object_vec* children = curr_obj->_children;

    if(children)
    {
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            _measure_aux(it_obj, scene);
        }
    }

    if(curr_obj->_aux)
    {
        ntg_object_measure(curr_obj);
    }
    else
    {
        for(i = 0; i < aux_children->_count; i++)
        {
            it_obj = aux_children->_data[i].object;
            _measure_aux(it_obj, scene);
        }
    }
}

static void _arrange_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    ntg_object_arrange(curr_obj);
    const ntg_object_vec* children = curr_obj->_children;

    if(children)
    {
        ntg_object* it_obj;
        size_t i;
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            _arrange_all(it_obj, scene);
        }
    }
}

static void _arrange_aux(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    size_t i;
    ntg_object* it_obj;

    const ntg_aux_object_vec* aux_children = curr_obj->_aux_children;
    const ntg_object_vec* children = curr_obj->_children;

    if(curr_obj->_aux)
    {
        ntg_object_arrange(curr_obj);
    }
    else
    {
        for(i = 0; i < aux_children->_count; i++)
        {
            it_obj = aux_children->_data[i].object;
            _arrange_aux(it_obj, scene);
        }
    }

    if(children)
    {
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            _arrange_aux(it_obj, scene);
        }
    }
}

static void _draw_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    ntg_scene* _scene = (ntg_scene*)scene;

    ntg_object_vec* children = curr_obj->_children;
    ntg_aux_object_vec* aux_children = curr_obj->_aux_children;

    size_t i, j;
    if(!(curr_obj->_aux))
    {
        struct ntg_aux_object* it_aux_object;
        for(i = 0; i < aux_children->_count; i++)
        {
            it_aux_object = &(aux_children->_data[i]);
            if(it_aux_object->z_index < NTG_AUX_OBJECT_Z_INDEX_DEFAULT)
            {
                it_aux_object = &(aux_children->_data[i]);
                _draw_all(it_aux_object->object, scene);
            }
        }
    }

    const ntg_object_drawing* obj_drawing = curr_obj->_drawing;
    if(obj_drawing != NULL)
    {
        struct ntg_xy obj_drawing_size = ntg_object_drawing_get_vp_size(obj_drawing);
        struct ntg_xy scene_drawing_size = ntg_scene_drawing_get_size(&_scene->_drawing);

        struct ntg_xy obj_pos = curr_obj->_pos;
        const ntg_cell* it_obj_cell;
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
                it_scene_cell = ntg_scene_drawing_at_(&_scene->_drawing, it_scene_pos);

                (*it_scene_cell) = ntg_cell_overwrite(*it_obj_cell, *it_scene_cell);
            }
        }
    }

    if(children)
    {
        ntg_object* it_obj;
        for(i = 0; i < children->_count; i++)
        {
            it_obj = children->_data[i];
            _draw_all(it_obj, scene);
        }
    }

    if(!(curr_obj->_aux))
    {
        struct ntg_aux_object* it_aux_object;
        for(i = 0; i < aux_children->_count; i++)
        {
            it_aux_object = &(aux_children->_data[i]);
            if(it_aux_object->z_index > NTG_AUX_OBJECT_Z_INDEX_DEFAULT)
            {
                it_aux_object = &(aux_children->_data[i]);
                _draw_all(it_aux_object->object, scene);
            }
        }
    }
}

static void _reset_scene_content(ntg_simple_scene* scene, struct ntg_xy size)
{
    ntg_scene* _scene = NTG_SCENE(scene);
    // struct ntg_xy size = ntg_scene_drawing_get_size(&_scene->_drawing);

    size_t i, j;
    struct ntg_rcell* it_rcell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_rcell = ntg_scene_drawing_at_(&_scene->_drawing, ntg_xy(j, i));
            (*it_rcell) = ntg_rcell_default();
            // (*it_rcell) = (struct ntg_rcell) {
            //     .codepoint = 'a',
            //     .gfx = (struct nt_gfx) {
            //         .bg = nt_color_new(255, 255, 255),
            //         .fg = nt_color_new(0, 0, 0),
            //         .style = NT_STYLE_BOLD
            //     }
            // };
        }
    }
}

static void __layout_fn(ntg_scene* _scene, struct ntg_xy size)
{
    ntg_simple_scene* scene = (ntg_simple_scene*)_scene;
    ntg_object* root = _scene->_root;
    if(root == NULL) return;

    ntg_object_layout_root(root, size);

    _nsize_all(root, scene);
    _constrain_all(root, scene);
    _measure_all(root, scene);
    _arrange_all(root, scene);

    _constrain_aux(root, scene);
    _measure_aux(root, scene);
    _arrange_aux(root, scene);

    _reset_scene_content(scene, size);
    _draw_all(root, scene);
}
