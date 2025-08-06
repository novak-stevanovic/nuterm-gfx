#include <assert.h>

#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"

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

ntg_object_vec* ntg_object_get_children(ntg_object* object)
{
    assert(object != NULL);

    return object->__children;
}

/* ---------------------------------------------------------------- */

struct ntg_xy ntg_object_get_natural_size(const ntg_object* object)
{
    assert(object != NULL);

    return object->__natural_size;
}

struct ntg_constr ntg_object_get_constraints(const ntg_object* object)
{
    assert(object != NULL);

    return object->__constraints;
}

struct ntg_constr ntg_object_get_adjusted_constraints(const ntg_object* object)
{
    assert(object != NULL);

    return object->__adjusted_constraints;
}

struct ntg_xy ntg_object_get_size(const ntg_object* object)
{
    assert(object != NULL);

    return object->__size;
}

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

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED API */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_object* object);

static uint __id_generator = 0;

void __ntg_object_init__(ntg_object* object,
        ntg_object_type type,
        ntg_natural_size_fn natural_size_fn,
        ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn,
        ntg_arrange_children_fn arrange_children_fn,
        ntg_arrange_drawing_fn arrange_drawing_fn)
{
    assert(object != NULL);

    __init_default_values(object);

    object->_id = __id_generator++;
    object->_type = type;

    object->__natural_size_fn = natural_size_fn;
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

void ntg_object_compute_natural_size(ntg_object* object)
{
    assert(object != NULL);

    struct ntg_xy natural_size = object->__natural_size_fn(object);

    struct ntg_xy adjusted_natural_size = ntg_xy_clamp(
            object->__min_size, natural_size,
            object->__max_size);

    object->__natural_size = natural_size;
    object->__adjusted_natural_size = adjusted_natural_size;
}

void ntg_object_constrain(ntg_object* object)
{
    assert(object != NULL);
}

void ntg_object_measure(ntg_object* object)
{
    assert(object != NULL);
}

void ntg_object_arrange_children(ntg_object* object)
{
    assert(object != NULL);
}

void ntg_object_arrange_drawing(ntg_object* object)
{
    assert(object != NULL);
}

/* ---------------------------------------------------------------- */

static void __init_default_values(ntg_object* object)
{
    object->_id = UINT_MAX;
    object->_type = NTG_OBJECT_WIDGET;

    object->__parent = NULL;
    object->__children = NULL;

    object->__natural_size_fn = NULL;
    object->__constrain_fn = NULL;
    object->__measure_fn = NULL;
    object->__arrange_children_fn = NULL;
    object->__arrange_drawing_fn = NULL;

    object->__natural_size = ntg_xy(0, 0);
    object->__adjusted_natural_size = ntg_xy(0, 0);
    object->__constraints = ntg_constr(ntg_xy(0, 0), ntg_xy(0, 0));
    object->__adjusted_constraints = ntg_constr(ntg_xy(0, 0), ntg_xy(0, 0));
    object->__size = ntg_xy(0, 0);
    object->__position = ntg_xy(0, 0);
    object->__drawing = NULL;

    object->__min_size = NTG_XY_MIN;
    object->__max_size = NTG_XY_MAX;

    object->__process_key_fn = NULL;
    object->__listenable = (ntg_listenable) {0};
}
