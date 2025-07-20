#include <assert.h>
#include <stdlib.h>

#include "core/ntg_simple_scene.h"
#include "base/ntg_event_types.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/_uthash.h"

/* -------------------------------------------------------------------------- */
/* HASHMAP */
/* -------------------------------------------------------------------------- */

struct object_data
{
    ntg_object* object;

    bool recalculate_natural_size;

    struct ntg_constr old_constr;
    bool reconstrain;

    bool remeasure;

    bool rearrange;

    bool redraw;

    UT_hash_handle hh;
};

static inline void __objects_data_add(struct object_data* hm, struct object_data* data)
{
    HASH_ADD_PTR(hm, object, data);
}

static inline struct object_data* __objects_data_get(struct object_data* hm, ntg_object* object)
{
    struct object_data* _data;

    HASH_FIND_PTR(hm, &object, _data);

    return _data;
}

static inline void __objects_data_remove(struct object_data* hm, struct object_data* data)
{
    HASH_DEL(hm, data);
    free(data);
}

static void __objects_data_destroy(struct object_data* hm)
{
    struct object_data *curr_data, *tmp;

    HASH_ITER(hh, hm, curr_data, tmp)
    {
        HASH_DEL(hm, curr_data);
        free(curr_data);
    }
}

/* -------------------------------------------------------------------------- */
/* SIMPLE SCENE */
/* -------------------------------------------------------------------------- */

static void __layout_fn(ntg_scene* _scene, struct ntg_xy size);
static void __on_object_register_fn(ntg_scene* _scene, ntg_object* object);
static void __on_object_unregister_fn(ntg_scene* _scene, ntg_object* object);

static void __object_handler(void* __scene, ntg_event* event);

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

    __objects_data_destroy(scene->__objects_data);
    __ntg_simple_scene_deinit__(scene);
    free(scene);
}

/* -------------------------------------------------------------------------- */

static void __natural_size_phase(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    struct object_data* data = __objects_data_get(scene->__objects_data, curr_obj);
    assert(data != NULL);

    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        __natural_size_phase(it_obj, scene);
    }

    if(data->recalculate_natural_size)
    {
        ntg_object_calculate_natural_size(curr_obj);

        struct ntg_xy old_nsize = ntg_object_get_natural_size(curr_obj);
        struct ntg_xy old_content_nsize = ntg_object_get_natural_content_size(curr_obj);

        struct ntg_xy new_nsize = ntg_object_get_natural_size(curr_obj);
        struct ntg_xy new_content_nsize = ntg_object_get_natural_content_size(curr_obj);

        if(!ntg_xy_are_equal(old_content_nsize, new_content_nsize))
        {
            data->reconstrain = true;
            data->remeasure = true;
        }

        if(!ntg_xy_are_equal(old_nsize, new_nsize) && (curr_obj->_parent != NULL))
        {
            struct object_data* parent_data =
                __objects_data_get(scene->__objects_data, curr_obj->_parent);

            parent_data->recalculate_natural_size = true;
        }

        data->recalculate_natural_size = false;
    }
}

static void __constrain_phase(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    struct object_data* data = __objects_data_get(scene->__objects_data, curr_obj);
    assert(data != NULL);
    const ntg_object_vec* children = curr_obj->_children;

    struct ntg_constr old_constr = data->old_constr;
    struct ntg_constr new_constr = ntg_object_get_constr(curr_obj);

    if(!ntg_constr_are_equal(old_constr, new_constr))
    {
        data->reconstrain = true;
        data->remeasure = true;
        data->old_constr = new_constr;
    }

    if(data->reconstrain)
    {
        ntg_object_constrain(curr_obj);
        data->reconstrain = false;
    }

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

    struct object_data* data = __objects_data_get(scene->__objects_data, curr_obj);
    assert(data != NULL);
    const ntg_object_vec* children = curr_obj->_children;

    ntg_object* it_obj;
    size_t i;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        __measure_phase(it_obj, scene);
    }

    struct ntg_xy old_size = ntg_object_get_size(curr_obj);
    struct ntg_xy old_content_size = ntg_object_get_content_size(curr_obj);
    /* TODO: fixed?
     * problem with syncing (natural) size/content size because border size is
     * determined at constr phase. Meaning:
     * 1. if object has certain content and border size,
     * 2. border_size changes - content_size calculation changes
     * 3. `old_content_size` will be invalid. */

    if(data->remeasure)
    {
        ntg_object_measure(curr_obj);
        data->remeasure = false;
    }

    struct ntg_xy new_size = ntg_object_get_size(curr_obj);
    struct ntg_xy new_content_size = ntg_object_get_content_size(curr_obj);

    if((!ntg_xy_are_equal(old_content_size, new_content_size)) ||
            !(ntg_xy_are_equal(old_size, new_size)))
    {
        data->rearrange = true;
    }
}

static void __arrange_phase(ntg_object* curr_obj, ntg_simple_scene* scene)
{
    if(curr_obj == NULL) return;

    struct object_data* data = __objects_data_get(scene->__objects_data, curr_obj);
    assert(data != NULL);
    const ntg_object_vec* children = curr_obj->_children;

    if(data->rearrange)
    {
        ntg_object_arrange(curr_obj);
        data->rearrange = false;
    }

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

    struct object_data* data = __objects_data_get(scene->__objects_data, curr_obj);
    assert(data != NULL);
    const ntg_object_vec* children = curr_obj->_children;

    ntg_scene* _scene = NTG_SCENE(scene);

    if(data->redraw)
    {
        ntg_object_drawing_place_(
                curr_obj->_full_drawing, ntg_xy(0, 0), ntg_object_get_size(curr_obj),
                &_scene->_drawing, ntg_object_get_pos_abs(curr_obj));
        
        data->redraw = false;
    }

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
    ntg_simple_scene* scene = (ntg_simple_scene*)_scene;

    struct object_data* data = (struct object_data*)malloc(
            sizeof(struct object_data));
    assert(data != NULL);

    (*data) = (struct object_data) {
        .object = object,
        .recalculate_natural_size = true,
        .old_constr = NTG_CONSTR_UNSET,
        .reconstrain = true,
        .remeasure = true,
        .rearrange = true,
        .redraw = false
    };

    __objects_data_add(scene->__objects_data, data);

    struct ntg_event_sub sub = {
        .subscriber = _scene,
        .handler = __object_handler
    };

    ntg_object_listen(object, sub);
}

static void __on_object_unregister_fn(ntg_scene* _scene, ntg_object* object)
{
    ntg_simple_scene* scene = (ntg_simple_scene*)_scene;

    struct object_data* data = __objects_data_get(scene->__objects_data, object);
    __objects_data_remove(scene->__objects_data, data);

    ntg_object_stop_listening(object, _scene);
}

static void __object_handler(void* __scene, ntg_event* event)
{
    ntg_scene* _scene = (ntg_scene*)__scene;
    ntg_simple_scene* scene = (ntg_simple_scene*)__scene;

    ntg_object* source = (ntg_object*)event->_source;
    struct object_data* data = __objects_data_get(scene->__objects_data, source);

    switch(event->_type)
    {
        case NTG_OBJECT_INTERNALS_CHANGE:
            data->recalculate_natural_size = true;
            data->reconstrain = true;
            data->remeasure = true;
            data->rearrange = true;
            data->redraw = true;
            break;
        case NTG_OBJECT_PREF_SIZE_CHANGE:
            data->recalculate_natural_size = true;
            data->reconstrain = true;
            data->remeasure = true;
            break;
    }
}
