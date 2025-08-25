#include <assert.h>

#include "object/ntg_layout_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"

struct layout_data
{
    SArena* arena;
};

static void __set_layout_orientation(ntg_object* object, void* _orientation);
static void __measure1_fn(ntg_object* object, void* _layout_data);
static void __constrain1_fn(ntg_object* object, void* _layout_data);
static void __measure2_fn(ntg_object* object, void* _layout_data);
static void __constrain2_fn(ntg_object* object, void* _layout_data);
static void __arrange_children_fn(ntg_object* object, void* _layout_data);
static void __arrange_drawing_fn(ntg_object* object, void* _layout_data);
static void __arrange_fn(ntg_object* object, void* _layout_data); // children & drawing

/* -------------------------------------------------------------------------- */

void __ntg_layout_object_init__(ntg_layout_object* layout_object,
       ntg_object* object, struct ntg_xy size)
{
    assert(layout_object != NULL);
    assert(object != NULL);

    layout_object->_object = object;
    layout_object->_size = size;
    layout_object->__arena = sarena_create(100000, NULL);
    assert(layout_object->__arena != NULL);
}

void __ntg_layout_object_deinit__(ntg_layout_object* layout_object)
{
    assert(layout_object != NULL);

    layout_object->_object = NULL;
    layout_object->_size = ntg_xy(0, 0);
    sarena_destroy(layout_object->__arena);
    layout_object->__arena = NULL;
}

void ntg_layout_object_perform(ntg_layout_object* layout_object)
{
    assert(layout_object != NULL);

    struct ntg_xy size = layout_object->_size;
    ntg_object* root = layout_object->_object;
    ntg_orientation orientation = NTG_ORIENTATION_HORIZONTAL;

    struct layout_data data = {
        .arena = layout_object->__arena,
    };

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __set_layout_orientation, &orientation);

    root->__size = size;
    root->__position = ntg_xy(0, 0);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_BOTTOM_UP,
            __measure1_fn, &data);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __constrain1_fn, &data);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_BOTTOM_UP,
            __measure2_fn, &data);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __constrain2_fn, &data);

    ntg_object_perform_tree(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            __arrange_fn, &data);

    sarena_rewind(layout_object->__arena);
}

/* -------------------------------------------------------------------------- */

static void __set_layout_orientation(ntg_object* object,
        void* _orientation)
{
    assert(object != NULL);
    
    ntg_orientation orientation = *(ntg_orientation*)_orientation;
    object->__layout_orientation = orientation;
}

static void __measure1_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data data = *(struct layout_data*)_layout_data;
    ntg_measure_context* context = ntg_measure_context_new(object, data.arena);
    ntg_object_vec* children = ntg_object_get_children_(object);

    size_t i;
    struct ntg_measure_data it_data;
    ntg_object* it_object;
    for(i = 0; i < children->_count; i++)
    {
        it_object = children->_data[i];

        it_data = (struct ntg_measure_data) {
            .min_size = it_object->__min_size.x,
            .natural_size = it_object->__natural_size.x,
            .max_size = it_object->__max_size.x
        };

        ntg_measure_context_set(context, it_object, it_data);
    }

    struct ntg_measure_result result = _ntg_object_measure(
            object, NTG_ORIENTATION_HORIZONTAL,
            SIZE_MAX, context);

    object->__min_size.x = result.min_size;
    object->__natural_size.x = result.natural_size;
    object->__max_size.x = result.max_size;

    ntg_measure_context_destroy(context);
}

static void __constrain1_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data data = *(struct layout_data*)_layout_data;
    ntg_constrain_context* context = ntg_constrain_context_new(object, data.arena);
    ntg_constrain_output* output = ntg_constrain_output_new(object, data.arena);
    ntg_object_vec* children = ntg_object_get_children_(object);
    
    size_t i;
    struct ntg_constrain_data it_data;
    ntg_object* it_object;
    for(i = 0; i < children->_count; i++)
    {
        it_object = children->_data[i];

        it_data = (struct ntg_constrain_data) {
            .min_size = it_object->__min_size.x,
            .natural_size = it_object->__natural_size.x,
            .max_size = it_object->__max_size.x
        };

        ntg_constrain_context_set(context, it_object, it_data);
    }

    ntg_constrain_context_set_min_size(context, object->__min_size.x);
    ntg_constrain_context_set_natural_size(context, object->__natural_size.x);

    size_t content_size = _ntg_object_constrain(object,
            NTG_ORIENTATION_HORIZONTAL, object->__size.x,
            context, output);

    for(i = 0; i < children->_count; i++)
    {
        it_object = children->_data[i];

        it_object->__size.x = ntg_constrain_output_get(output, it_object).size;
    }

    object->__content_size.x = content_size;

    ntg_constrain_context_destroy(context);
    ntg_constrain_output_destroy(output);
}

static void __measure2_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data data = *(struct layout_data*)_layout_data;
    ntg_measure_context* context = ntg_measure_context_new(object, data.arena);
    ntg_object_vec* children = ntg_object_get_children_(object);

    size_t i;
    struct ntg_measure_data it_data;
    ntg_object* it_object;
    for(i = 0; i < children->_count; i++)
    {
        it_object = children->_data[i];

        it_data = (struct ntg_measure_data) {
            .min_size = it_object->__min_size.y,
            .natural_size = it_object->__natural_size.y,
            .max_size = it_object->__natural_size.y
        };

        ntg_measure_context_set(context, it_object, it_data);
    }

    struct ntg_measure_result result = _ntg_object_measure(
            object, NTG_ORIENTATION_VERTICAL,
            object->__size.x, context);

    object->__min_size.y = result.min_size;
    object->__natural_size.y = result.natural_size;
    object->__max_size.y = result.max_size;

    ntg_measure_context_destroy(context);
}

static void __constrain2_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data data = *(struct layout_data*)_layout_data;
    ntg_constrain_context* context = ntg_constrain_context_new(object, data.arena);
    ntg_constrain_output* output = ntg_constrain_output_new(object, data.arena);
    ntg_object_vec* children = ntg_object_get_children_(object);
    
    size_t i;
    struct ntg_constrain_data it_data;
    ntg_object* it_object;
    for(i = 0; i < children->_count; i++)
    {
        it_object = children->_data[i];

        it_data = (struct ntg_constrain_data) {
            .min_size = it_object->__min_size.y,
            .max_size = it_object->__max_size.y,
            .natural_size = it_object->__natural_size.y
        };

        ntg_constrain_context_set(context, it_object, it_data);
    }

    ntg_constrain_context_set_min_size(context, object->__min_size.y);
    ntg_constrain_context_set_natural_size(context, object->__natural_size.y);

    size_t content_size = _ntg_object_constrain(object,
            NTG_ORIENTATION_VERTICAL, object->__size.y,
            context, output);

    for(i = 0; i < children->_count; i++)
    {
        it_object = children->_data[i];

        it_object->__size.y = ntg_constrain_output_get(output, it_object).size;
    }
    
    object->__content_size.y = content_size;

    ntg_constrain_context_destroy(context);
    ntg_constrain_output_destroy(output);
}

static void __arrange_children_fn(ntg_object* object, void* _layout_data)
{
    struct layout_data data = *(struct layout_data*)_layout_data;
    ntg_arrange_context* context = ntg_arrange_context_new(object, data.arena);
    ntg_arrange_output* output = ntg_arrange_output_new(object, data.arena);
    ntg_object_vec* children = ntg_object_get_children_(object);

    size_t i;
    struct ntg_arrange_data it_data;
    ntg_object* it_object;
    for(i = 0; i < children->_count; i++)
    {
        it_object = children->_data[i];

        it_data = (struct ntg_arrange_data) {
            .size = it_object->__size
        };

        ntg_arrange_context_set(context, it_object, it_data);
    }

    ntg_arrange_context_set_content_size(context, object->__content_size);

    _ntg_object_arrange_children(object, object->__size, context, output);

    for(i = 0; i < children->_count; i++)
    {
        it_object = children->_data[i];

        it_object->__position = ntg_arrange_output_get(output, it_object).pos;
    }

    ntg_arrange_context_destroy(context);
    ntg_arrange_output_destroy(output);
}

static void __arrange_drawing_fn(ntg_object* object, void* _layout_data)
{
    ntg_object_drawing_set_size(object->__drawing, object->__size);

    _ntg_object_arrange_drawing(object, object->__size, object->__drawing);
}

static void __arrange_fn(ntg_object* object, void* _layout_data) // children & drawing
{
    __arrange_children_fn(object, _layout_data);
    __arrange_drawing_fn(object, _layout_data);
}
