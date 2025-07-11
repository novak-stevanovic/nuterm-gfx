#include "core/ntg_simple_scene.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include <assert.h>
#include <stdlib.h>

static void __layout_fn(ntg_scene* _scene, struct ntg_xy size);

void __ntg_simple_scene_init__(ntg_simple_scene* scene)
{
    assert(scene != NULL);

    __ntg_scene_init__((ntg_scene*)scene, __layout_fn);
}

void __ntg_simple_scene_deinit__(ntg_simple_scene* scene)
{
    assert(scene != NULL);

    __ntg_scene_deinit__((ntg_scene*)scene);
}

ntg_simple_scene* ntg_simple_scene_new()
{
    ntg_simple_scene* new = (ntg_simple_scene*)malloc(
            sizeof(struct ntg_simple_scene));

    assert(new != NULL);

    __ntg_simple_scene_init__(new);

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

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _nsize_all(it_obj, scene);
    }

    ntg_object_calculate_nsize(curr_obj);
}

static void _constrain_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    ntg_object_constrain(curr_obj);
    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _constrain_all(it_obj, scene);
    }
}

static void _measure_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _measure_all(it_obj, scene);
    }
    ntg_object_measure(curr_obj);
}

static void _arrange_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    ntg_object_arrange(curr_obj);
    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _arrange_all(it_obj, scene);
    }
}

static void _draw_all(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    ntg_scene* _scene = NTG_SCENE(scene);

    const ntg_object_vec* children = curr_obj->_children;

    ntg_object_drawing_place_(
            curr_obj->_full_drawing, ntg_xy(0, 0), ntg_object_get_size(curr_obj),
            &_scene->_drawing, ntg_object_get_pos_abs(curr_obj));

    size_t i;
    ntg_object* it_obj;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        _draw_all(it_obj, scene);
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
    _draw_all(root, scene);
}
