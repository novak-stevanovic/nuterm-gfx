#include <assert.h>

#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "object/shared/ntg_object_xy_map.h"
#include "object/shared/ntg_object_size_map.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_object_type ntg_object_get_type(const ntg_object* object)
{
    assert(object != NULL);

    return object->_type;
}

ntg_object* ntg_object_get_parent(ntg_object* object,
        ntg_object_get_parent_mode mode)
{
    assert(object != NULL);

    ntg_object* top_decorator;
    switch(mode)
    {
        case NTG_OBJECT_GET_PARENT_INC_DECORATOR:
            return object->__parent;
        case NTG_OBJECT_GET_PARENT_EXC_DECORATOR:
             top_decorator = ntg_object_get_top_decorator(object);

             if(top_decorator == NULL) return object->__parent;
             else return (top_decorator->__parent);
    }
}

ntg_object* ntg_object_get_decorator(ntg_object* object)
{
    assert(object != NULL);

    if(object->__parent == NULL) return NULL;

    return ((object->__parent->_type != NTG_OBJECT_WIDGET) ? object->__parent : NULL);
}

ntg_object* ntg_object_get_top_decorator(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* prev_obj = object;
    ntg_object* it_obj = object->__parent;

    while((it_obj != NULL) && (it_obj->_type == NTG_OBJECT_DECORATOR))
    {
        prev_obj = it_obj;
        it_obj = it_obj->__parent;
    }

    return (prev_obj->_type == NTG_OBJECT_DECORATOR) ? prev_obj : NULL;
}

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

struct ntg_xy ntg_object_get_position(const ntg_object* object)
{
    assert(object != NULL);

    return object->__position;
}

const ntg_object_drawing* ntg_object_get_drawing(const ntg_object* object)
{
    assert(object != NULL);

    return ((const ntg_object_drawing*)object->__drawing);
}

/* ---------------------------------------------------------------- */

static void __set_layout_orientation(ntg_object* object,
        void* _orientation);
static void __measure1_fn(ntg_object* object, void* _);
static void __constrain1_fn(ntg_object* object, void* _);
static void __measure2_fn(ntg_object* object, void* _);
static void __constrain2_fn(ntg_object* object, void* _);
static void __arrange_fn(ntg_object* object, void* _); // children & drawing
static void __arrange_children_fn(ntg_object* object, void* _);
static void __arrange_drawing_fn(ntg_object* object, void* _);

void ntg_object_layout(ntg_object* root, struct ntg_xy size,
        ntg_orientation layout_orientation)
{
    assert(root != NULL);

    struct ntg_oxy _size = ntg_oxy_from_xy(size, layout_orientation);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __set_layout_orientation, &layout_orientation);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_BOTTOM_UP,
            __measure1_fn, NULL);

    root->__size.prim_val = _size.prim_val;
    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __constrain1_fn, NULL);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_BOTTOM_UP,
            __measure2_fn, NULL);

    root->__size.sec_val = _size.sec_val;
    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __constrain2_fn, NULL);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __arrange_fn, NULL);
}

static void __set_layout_orientation(ntg_object* object,
        void* _orientation)
{
    assert(object != NULL);
    
    ntg_orientation orientation = *(ntg_orientation*)_orientation;
    object->__layout_orientation = orientation;
}

static void __measure1_fn(ntg_object* object, void* _)
{
    assert(object != NULL);

    struct ntg_measure_result data = object->__measure_fn(
            object, object->__layout_orientation, SIZE_MAX);

    object->__min_size.prim_val = data.min_size;
    object->__natural_size.prim_val = data.natural_size;
}

static void __constrain1_fn(ntg_object* object, void* _)
{
    assert(object != NULL);

    ntg_object_vec* children = object->__children;
    if((children == NULL) || (children->_count == 0))
        return;

    ntg_object_size_map sizes;
    __ntg_object_size_map_init__(&sizes, object);

    object->__constrain_fn(object, 
            object->__layout_orientation,
            object->__size.prim_val, &sizes);

    size_t i;
    ntg_object* it_obj;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        // TODO: check if correct
        it_obj->__size.prim_val = ntg_object_size_map_get(&sizes, it_obj);
    }

    __ntg_object_size_map_deinit__(&sizes);
}

static void __measure2_fn(ntg_object* object, void* _)
{
    assert(object != NULL);

    struct ntg_measure_result data = object->__measure_fn(object,
            ntg_orientation_get_other(object->__layout_orientation),
            object->__size.prim_val);

    object->__min_size.sec_val = data.min_size;
    object->__natural_size.sec_val = data.natural_size;
}

static void __constrain2_fn(ntg_object* object, void* _)
{
    assert(object != NULL);

    ntg_object_vec* children = object->__children;
    if((children == NULL) || (children->_count == 0))
        return;

    ntg_object_size_map sizes;
    __ntg_object_size_map_init__(&sizes, object);

    object->__constrain_fn(object, 
            ntg_orientation_get_other(object->__layout_orientation),
            object->__size.sec_val, &sizes);

    size_t i;
    ntg_object* it_obj;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        // TODO: check if correct
        it_obj->__size.sec_val = ntg_object_size_map_get(&sizes, it_obj);
    }

    __ntg_object_size_map_deinit__(&sizes);
}

static void __arrange_fn(ntg_object* object, void* _)
{
    assert(object != NULL);

    __arrange_children_fn(object, _);
    __arrange_drawing_fn(object, _);
}

static void __arrange_children_fn(ntg_object* object, void* _)
{
    assert(object != NULL);

    ntg_object_vec* children = object->__children;
    if((children == NULL) || (children->_count == 0))
        return;

    ntg_object_xy_map _positions;
    __ntg_object_xy_map_init__(&_positions, object);

    struct ntg_xy size = ntg_xy_from_oxy(object->__size,
            object->__layout_orientation);

    object->__arrange_children_fn(object, size, &_positions);

    size_t i;
    ntg_object* it_obj;
    for(i = 0; i < children->_count; i++)
    {
        it_obj = children->_data[i];
        // TODO: check if correct
        it_obj->__position = ntg_object_xy_map_get(&_positions, it_obj);
    }

    __ntg_object_xy_map_deinit__(&_positions);
}

static void __arrange_drawing_fn(ntg_object* object, void* _)
{
    assert(object != NULL);

    struct ntg_xy size = ntg_xy_from_oxy(object->__size,
            object->__layout_orientation);

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(object->__drawing, ntg_xy(j, i));
            (*it_cell) = object->__bg;
        }
    }

    if(object->__arrange_drawing_fn)
        object->__arrange_drawing_fn(object, size, object->__drawing);
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

void ntg_object_perform_tree(ntg_object* object,
        ntg_object_perform_mode mode,
        void (*perform_fn)(ntg_object* object, void* data),
        void* data)
{
    assert(object != NULL);
    assert(perform_fn != NULL);

    if(mode == NTG_OBJECT_PERFORM_TOP_DOWN)
    {
        perform_fn(object, data);
    }

    size_t i;
    for(i = 0; i < object->__children->_count; i++)
    {
        ntg_object_perform_tree(object->__children->_data[i], mode,
                perform_fn, data);
    }

    if(mode == NTG_OBJECT_PERFORM_BOTTOM_UP)
    {
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

void _ntg_object_set_bg(ntg_object* object, ntg_cell bg)
{
    assert(object != NULL);

    object->__bg = bg;
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

    object->__position = ntg_xy(0, 0);
    object->__drawing = NULL;

    object->__layout_orientation = NTG_ORIENTATION_HORIZONTAL;
    object->__min_size = ntg_oxy(0, 0);
    object->__natural_size = ntg_oxy(0, 0);
    object->__max_size = ntg_oxy(0, 0);
    object->__size = ntg_oxy(0, 0);

    object->__process_key_fn = NULL;
    object->__listenable = (ntg_listenable) {0};
}
