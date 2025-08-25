#include <assert.h>

#include "object/ntg_object.h"
#include "core/ntg_scene.h"
#include "object/ntg_layout_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "object/shared/ntg_object_xy_map.h"
#include "object/shared/ntg_object_size_map.h"
#include "shared/_ntg_shared.h"

static void __update_scene_fn(ntg_object* object, void* _scene);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_object_type ntg_object_get_type(const ntg_object* object)
{
    assert(object != NULL);

    return object->_type;
}

ntg_object* ntg_object_get_group_root(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    while(true)
    {
        if((it_obj->__parent == NULL) ||
                (it_obj->__parent->_type == NTG_OBJECT_WIDGET))
        {
            return it_obj;
        }

        it_obj = it_obj->__parent;
    }
}

ntg_object* ntg_object_get_parent(ntg_object* object,
        ntg_object_get_parent_mode mode)
{
    assert(object != NULL);

    ntg_object* group_root;
    switch(mode)
    {
        case NTG_OBJECT_GET_PARENT_INCL_DECORATOR:
            return object->__parent;
        case NTG_OBJECT_GET_PARENT_EXCL_DECORATOR:
             group_root = ntg_object_get_group_root(object);

             return group_root->__parent;
        default:
             assert(0);
    }
}

// ntg_object* ntg_object_get_decorator(ntg_object* object)
// {
//     assert(object != NULL);
//
//     if(object->__parent == NULL) return NULL;
//
//     return ((object->__parent->_type != NTG_OBJECT_WIDGET) ? object->__parent : NULL);
// }
//
// ntg_object* ntg_object_get_top_decorator(ntg_object* object)
// {
//     assert(object != NULL);
//
//     ntg_object* prev_obj = object;
//     ntg_object* it_obj = object->__parent;
//
//     while((it_obj != NULL) && (it_obj->_type == NTG_OBJECT_DECORATOR))
//     {
//         prev_obj = it_obj;
//         it_obj = it_obj->__parent;
//     }
//
//     return (prev_obj->_type == NTG_OBJECT_DECORATOR) ? prev_obj : NULL;
// }

ntg_object* ntg_object_get_base_widget(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    while(it_obj->_type == NTG_OBJECT_DECORATOR)
        it_obj = it_obj->__children->_data[0];

    return it_obj;
}

ntg_object_vec* ntg_object_get_children_(ntg_object* object)
{
    assert(object != NULL);

    return object->__children;
}

const ntg_object_vec* ntg_object_get_children(const ntg_object* object)
{
    assert(object != NULL);

    return object->__children;
}

/* ---------------------------------------------------------------- */

void ntg_object_set_min_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__min_size = size;
    object->__set_min_size = true;
}

void ntg_object_set_natural_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__natural_size = size;
    object->__set_natural_size = true;
}

void ntg_object_set_max_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__max_size = size;
    object->__set_max_size = true;
}

void ntg_object_unset_min_size(ntg_object* object)
{
    assert(object != NULL);

    object->__set_min_size = false;
}

void ntg_object_unset_natural_size(ntg_object* object)
{
    assert(object != NULL);

    object->__set_natural_size = false;
}

void ntg_object_unset_max_size(ntg_object* object)
{
    assert(object != NULL);

    object->__set_max_size = false;
}

struct ntg_xy ntg_object_get_min_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__min_size;
}

struct ntg_xy ntg_object_get_natural_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__natural_size;
}

struct ntg_xy ntg_object_get_max_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__max_size;
}

/* ---------------------------------------------------------------- */

void ntg_object_layout(ntg_object* root, struct ntg_xy size)
{
    ntg_layout_object layout_object;
    __ntg_layout_object_init__(&layout_object, root, size);

    ntg_layout_object_perform(&layout_object);

    __ntg_layout_object_deinit__(&layout_object);
}

/* ---------------------------------------------------------------- */

struct ntg_xy ntg_object_get_size(const ntg_object* object)
{
    return object->__size;
}

struct ntg_xy ntg_object_get_position_abs(const ntg_object* object)
{
    struct ntg_xy position = ntg_xy(0, 0);

    const ntg_object* it_obj = object;
    while(it_obj != NULL)
    {
        position = ntg_xy_add(position, object->__position);
        it_obj = it_obj->__parent;
    }

    return position;
}

struct ntg_xy ntg_object_get_position_rel(const ntg_object* object)
{
    return object->__position;
}

ntg_scene* ntg_object_get_scene(ntg_object* object)
{
    assert(object != NULL);

    return object->__scene;
}

const ntg_object_drawing* ntg_object_get_drawing(const ntg_object* object)
{
    return object->__drawing;
}

ntg_object_drawing* ntg_object_get_drawing_(ntg_object* object)
{
    return object->__drawing;
}

/* ---------------------------------------------------------------- */

void ntg_object_listen(ntg_object* object, struct ntg_event_sub subscription)
{
    assert(object != NULL);

    ntg_listenable_listen(&object->__listenable, subscription);
}

void ntg_object_stop_listening(ntg_object* object, void* subscriber)
{
    assert(object != NULL);

    ntg_listenable_stop_listening(&object->__listenable, subscriber);
}

/* ---------------------------------------------------------------- */

bool ntg_object_feed_key(ntg_object* object, struct nt_key_event key_event)
{
    return object->__process_key_fn(object, key_event);
}

/* ---------------------------------------------------------------- */

void ntg_object_perform_tree(ntg_object* object,
        ntg_object_perform_mode mode,
        void (*perform_fn)(ntg_object* object, void* data),
        void* data)
{
    assert(object != NULL);
    assert(perform_fn != NULL);

    size_t i;
    if(mode == NTG_OBJECT_PERFORM_TOP_DOWN)
    {
        perform_fn(object, data);

        for(i = 0; i < object->__children->_count; i++)
        {
            ntg_object_perform_tree(object->__children->_data[i],
                    mode, perform_fn, data);
        }
    }
    else
    {
        for(i = 0; i < object->__children->_count; i++)
        {
            ntg_object_perform_tree(object->__children->_data[i],
                    mode, perform_fn, data);
        }

        perform_fn(object, data);
    }
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED API */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_object* object);

static uint __id_generator = 0;

void __ntg_object_init__(ntg_object* object,
        ntg_object_type type,
        ntg_measure_fn measure_fn,
        ntg_constrain_fn constrain_fn,
        ntg_arrange_children_fn arrange_children_fn,
        ntg_arrange_drawing_fn arrange_drawing_fn)
{
    assert(object != NULL);

    __init_default_values(object);

    object->_id = __id_generator++;
    object->_type = type;

    object->__constrain_fn = constrain_fn;
    object->__measure_fn = measure_fn;
    object->__arrange_children_fn = arrange_children_fn;
    object->__arrange_drawing_fn = arrange_drawing_fn;

    object->__children = ntg_object_vec_new();
    object->__drawing = ntg_object_drawing_new();
    __ntg_listenable_init__(&object->__listenable);
}

void __ntg_object_deinit__(ntg_object* object)
{
    assert(object != NULL);

    ntg_object_vec_destroy(object->__children);
    ntg_object_drawing_destroy(object->__drawing);
    __ntg_listenable_deinit__(&object->__listenable);

    __init_default_values(object);
}

struct ntg_measure_result _ntg_object_measure(ntg_object* object,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    assert(object != NULL);
    assert(context != NULL);

    if(object->__measure_fn == NULL)
        return (struct ntg_measure_result) {0};

    struct ntg_measure_result result = object->__measure_fn(
            object, orientation, for_size, context);

    if(object->__set_min_size)
    {
        result.min_size = (orientation == NTG_ORIENTATION_HORIZONTAL) ?
            object->__min_size.x : object->__min_size.y;
    }
    if(object->__set_natural_size)
    {
        result.natural_size = (orientation == NTG_ORIENTATION_HORIZONTAL) ?
            object->__natural_size.x : object->__natural_size.y;
    }
    if(object->__set_max_size)
    {
        result.max_size = (orientation == NTG_ORIENTATION_HORIZONTAL) ?
            object->__max_size.x : object->__max_size.y;
    }

    return (struct ntg_measure_result) {
        .min_size = result.min_size,
        .natural_size = _max2_size(result.min_size, result.natural_size),
        .max_size = _max3_size(result.min_size, result.natural_size, result.max_size)
    };
}

size_t _ntg_object_constrain(ntg_object* object,
        ntg_orientation orientation, size_t size,
        const ntg_constrain_context* context,
        ntg_constrain_output* output)
{
    assert(object != NULL);
    assert(context != NULL);
    assert(output != NULL);

    if(object->__constrain_fn  == NULL)
        return 0;

    size_t result = object->__constrain_fn(object, orientation,
            size, context, output);

    return result;
}

void _ntg_object_arrange_children(ntg_object* object, struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* output)
{
    assert(object != NULL);
    assert(context != NULL);
    assert(output != NULL);

    if(object->__arrange_children_fn == NULL) return;

    object->__arrange_children_fn(object, size, context, output);
}

void _ntg_object_arrange_drawing(ntg_object* object, struct ntg_xy size,
        ntg_object_drawing* drawing)
{
    assert(object != NULL);
    assert(drawing != NULL);

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(drawing, ntg_xy(j, i)); 
            (*it_cell) = object->__background;
        }
    }

    if(object->__arrange_drawing_fn == NULL) return;

    object->__arrange_drawing_fn(object, size, drawing);
}

void _ntg_object_set_background(ntg_object* object, ntg_cell background)
{
    assert(object != NULL);

    object->__background = background;
}

void _ntg_object_root_set_scene(ntg_object* root, ntg_scene* scene)
{
    assert(root != NULL);

    ntg_object* parent = ntg_object_get_parent(root,
            NTG_OBJECT_GET_PARENT_EXCL_DECORATOR);
    assert(parent == NULL);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __update_scene_fn, scene);
}

void _ntg_object_add_child(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    assert(child->__parent == NULL);

    ntg_object_vec_append(object->__children, child);
    ntg_object_perform_tree(child, NTG_OBJECT_PERFORM_TOP_DOWN,
            __update_scene_fn, object->__scene);
}

void _ntg_object_rm_child(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    if(child->__parent != object) return;

    ntg_object_vec_remove(object->__children, child);

    child->__parent = NULL;

    ntg_object_perform_tree(object, NTG_OBJECT_PERFORM_TOP_DOWN,
            __update_scene_fn, NULL);
}

void _ntg_object_set_process_key_fn(ntg_object* object,
        ntg_object_process_key_fn process_key_fn)
{
    assert(object != NULL);

    object->__process_key_fn = process_key_fn;
}

ntg_object_drawing* _ntg_object_get_drawing(ntg_object* object)
{
    assert(object != NULL);

    return object->__drawing;
}

/* ---------------------------------------------------------------- */

static void __init_default_values(ntg_object* object)
{
    object->_id = UINT_MAX;
    object->_type = NTG_OBJECT_WIDGET;

    object->__parent = NULL;
    object->__children = NULL;

    object->__constrain_fn = NULL;
    object->__measure_fn = NULL;
    object->__arrange_children_fn = NULL;
    object->__arrange_drawing_fn = NULL;

    object->__drawing = NULL;
    object->__background = ntg_cell_default();

    object->__min_size = ntg_xy(0, 0);
    object->__natural_size = ntg_xy(0, 0);
    object->__max_size = ntg_xy(SIZE_MAX, SIZE_MAX);
    object->__content_size = ntg_xy(0, 0);
    object->__size = ntg_xy(0, 0);
    object->__set_min_size = false;
    object->__set_natural_size = false;
    object->__set_max_size = false;
    object->__position = ntg_xy(0, 0);

    object->__scene = NULL;

    object->__process_key_fn = NULL;
    object->__listenable = (ntg_listenable) {0};
}

/* ---------------------------------------------------------------- */

static void __update_scene_fn(ntg_object* object, void* _scene)
{
    ntg_scene* scene = (ntg_scene*)_scene;

    if(object->__scene != NULL)
        ntg_scene_unregister_object(object->__scene, object);

    if(scene != NULL)
        ntg_scene_register_object(scene, object);

    object->__scene = scene;
}
