#include <assert.h>
#include <stdlib.h>

#include "core/ntg_simple_scene.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"

struct object_data
{
    ntg_object* object;

    bool recalculate_natural_size;
    bool reconstrain;
    bool remeasure;
    bool rearrange;
};

static void __layout_fn(ntg_scene* _scene, struct ntg_xy size);
static void __on_object_register_fn(ntg_scene* _scene, ntg_object* object);
static void __on_object_unregister_fn(ntg_scene* _scene, ntg_object* object);

void __ntg_simple_scene_init__(ntg_simple_scene* scene)
{
    assert(scene != NULL);

    __ntg_scene_init__((ntg_scene*)scene,
            __layout_fn,
            __on_object_register_fn,
            __on_object_unregister_fn);
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

static void __natural_size_phase(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        __natural_size_phase(it_obj, scene);
    }

    ntg_object_calculate_natural_size(curr_obj);
}

static void __constrain_phase(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    ntg_object_constrain(curr_obj);
    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        __constrain_phase(it_obj, scene);
    }
}

static void __measure_phase(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        __measure_phase(it_obj, scene);
    }
    ntg_object_measure(curr_obj);
}

static void __arrange_phase(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    ntg_object_arrange(curr_obj);
    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        __arrange_phase(it_obj, scene);
    }
}

static void __draw_all(ntg_object* curr_obj, ntg_simple_scene* scene)
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
        __draw_all(it_obj, scene);
    }
}

static void __layout_fn(ntg_scene* _scene, struct ntg_xy size)
{
    ntg_simple_scene* scene = (ntg_simple_scene*)_scene;
    ntg_object* root = _scene->_root;
    if(root == NULL) return;

    ntg_object_layout_root(root, size);

    __natural_size_phase(root, scene);
    __constrain_phase(root, scene);
    __measure_phase(root, scene);
    __arrange_phase(root, scene);
    __draw_all(root, scene);
}

static void __on_object_register_fn(ntg_scene* _scene, ntg_object* object)
{
}

static void __on_object_unregister_fn(ntg_scene* _scene, ntg_object* object)
{
}
