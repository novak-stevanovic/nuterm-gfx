#include <assert.h>

#include "core/object/ntg_object.h"
#include "base/event/ntg_event.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/scene/ntg_drawable.h"
#include "core/scene/shared/ntg_drawable_kit.h"

#include "core/scene/shared/ntg_drawable_vec.h"
#include "shared/_ntg_shared.h"

static struct ntg_measure_output __ntg_object_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_context* context);

static void __ntg_object_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        struct ntg_measure_output measure_output,
        const ntg_constrain_context* context,
        ntg_constrain_output* output);

static void __ntg_object_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* output);

static void __ntg_object_draw_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        ntg_drawing* out_drawing);

static bool __ntg_object_process_key_fn(
        ntg_drawable* drawable,
        struct nt_key_event key_event,
        struct ntg_process_key_context context);

static void __ntg_object_on_focus_fn(
        ntg_drawable* drawable,
        struct ntg_focus_context context);

static ntg_drawable_vec_view __ntg_object_get_children_fn_(
        ntg_drawable* drawable);
static const ntg_drawable_vec* __ntg_object_get_children_fn(
        const ntg_drawable* drawable);

static ntg_drawable* __ntg_object_get_parent_fn_(
        ntg_drawable* drawable);
static const ntg_drawable* __ntg_object_get_parent_fn(
        const ntg_drawable* drawable);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_object_type ntg_object_get_type(const ntg_object* object)
{
    assert(object != NULL);

    return object->__type;
}

uint ntg_object_get_id(const ntg_object* object)
{
    assert(object != NULL);

    return object->__id;
}

ntg_object* ntg_object_get_group_root(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    while(true)
    {
        if((it_obj->__parent == NULL) ||
                (it_obj->__parent->__type == NTG_OBJECT_WIDGET))
        {
            return it_obj;
        }

        it_obj = it_obj->__parent;
    }
}

ntg_object* ntg_object_get_parent(ntg_object* object,
        ntg_object_get_parent_opts opts)
{
    assert(object != NULL);

    ntg_object* group_root;
    switch(opts)
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

ntg_object* ntg_object_get_base_widget(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    while(it_obj->__type == NTG_OBJECT_DECORATOR)
        it_obj = it_obj->__children->_data[0];

    return it_obj;
}

ntg_object_vec_view ntg_object_get_children_(ntg_object* object)
{
    assert(object != NULL);

    ntg_object_vec_view view;
    __ntg_object_vec_view_init__(&view, object->__children);

    return view;
}

const ntg_object_vec* ntg_object_get_children(const ntg_object* object)
{
    assert(object != NULL);

    return object->__children;
}

bool ntg_object_is_ancestor(ntg_object* object, ntg_object* ancestor)
{
    assert(object != NULL);
    assert(ancestor != NULL);

    return ntg_drawable_is_ancestor(
            &(object->__drawable),
            &(ancestor->__drawable));
}

bool ntg_object_is_descendant(ntg_object* object, ntg_object* descendant)
{
    assert(object != NULL);
    assert(descendant != NULL);

    return ntg_drawable_is_descendant(
            &(object->__drawable),
            &(descendant->__drawable));
}

/* ---------------------------------------------------------------- */

void ntg_object_set_min_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__min_size = size;
}

void ntg_object_set_max_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__max_size = size;
}

struct ntg_xy ntg_object_get_min_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__min_size;
}

struct ntg_xy ntg_object_get_max_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__max_size;
}

void ntg_object_set_grow(ntg_object* object, struct ntg_xy grow)
{
    assert(object != NULL);

    object->__grow = grow;
}

struct ntg_xy ntg_object_get_grow(const ntg_object* object)
{
    assert(object != NULL);
    
    return object->__grow;
}

/* ---------------------------------------------------------------- */

ntg_drawable* ntg_object_get_drawable_(ntg_object* object)
{
    assert(object != NULL);

    return &(object->__drawable);
}

const ntg_drawable* ntg_object_get_drawable(const ntg_object* object)
{
    assert(object != NULL);

    return &(object->__drawable);
}

ntg_listenable* ntg_object_get_listenable(ntg_object* object)
{
    assert(object != NULL);

    return ntg_event_delegate_listenable(object->__del);
}

/* ---------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED API */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_object* object);

static uint __id_generator = 0;

void __ntg_object_init__(ntg_object* object,
        ntg_object_type type,
        ntg_measure_fn measure_fn,
        ntg_constrain_fn constrain_fn,
        ntg_arrange_fn arrange_fn,
        ntg_draw_fn draw_fn,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn)
{
    assert(object != NULL);

    __init_default_values(object);

    object->__id = __id_generator++;
    object->__type = type;

    object->__children = ntg_object_vec_new();
    object->__children_drawables = ntg_drawable_vec_new();

    object->__wrapped_measure_fn = measure_fn;
    object->__wrapped_constrain_fn = constrain_fn;
    object->__wrapped_arrange_fn = arrange_fn;
    object->__wrapped_draw_fn = draw_fn;
    object->__wrapped_process_key_fn = process_key_fn;
    object->__wrapped_on_focus_fn = on_focus_fn;

    __ntg_drawable_init__(
            &(object->__drawable),
            object,
            __ntg_object_measure_fn,
            __ntg_object_constrain_fn,
            __ntg_object_arrange_fn,
            __ntg_object_draw_fn,
            __ntg_object_get_children_fn_,
            __ntg_object_get_children_fn,
            __ntg_object_get_parent_fn_,
            __ntg_object_get_parent_fn,
            __ntg_object_process_key_fn,
            __ntg_object_on_focus_fn);

    object->__del = ntg_event_delegate_new();
}

void __ntg_object_deinit__(ntg_object* object)
{
    assert(object != NULL);

    ntg_object_vec_destroy(object->__children);
    ntg_drawable_vec_destroy(object->__children_drawables);
    ntg_event_delegate_destroy(object->__del);

    __init_default_values(object);
}

void _ntg_object_set_background(ntg_object* object, ntg_cell background)
{
    assert(object != NULL);

    object->__background = background;
}

void _ntg_object_add_child(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    assert(child->__parent == NULL);

    ntg_object_vec_append(object->__children, child);
    ntg_drawable_vec_append(object->__children_drawables, &(child->__drawable));

    child->__parent = object;
}

void _ntg_object_rm_child(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    if(child->__parent != object) return;

    ntg_object_vec_remove(object->__children, child);
    ntg_drawable_vec_remove(object->__children_drawables, &(child->__drawable));

    child->__parent = NULL;
}

/* ---------------------------------------------------------------- */

static void __init_default_values(ntg_object* object)
{
    object->__id = UINT_MAX;
    object->__type = NTG_OBJECT_WIDGET;

    object->__parent = NULL;
    object->__children = NULL;
    object->__children_drawables= NULL;

    object->__background = ntg_cell_default();

    object->__grow = ntg_xy(1, 1);
    object->__min_size = ntg_xy(0, 0);
    object->__max_size = ntg_xy(NTG_SIZE_MAX, NTG_SIZE_MAX);

    object->__wrapped_measure_fn = NULL;
    object->__wrapped_constrain_fn = NULL;
    object->__wrapped_arrange_fn = NULL;
    object->__wrapped_draw_fn = NULL;
    object->__wrapped_process_key_fn = NULL;
    object->__wrapped_on_focus_fn = NULL;

    object->__drawable = (ntg_drawable) {0};
    object->__del = NULL;
}

/* ---------------------------------------------------------------- */

static struct ntg_measure_output __ntg_object_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_context* context)
{
    assert(drawable != NULL);
    assert(context != NULL);
    assert(for_size != 0);
    // if(for_size == 0) return (struct ntg_measure_output) {0};

    const ntg_object* object = ntg_drawable_user(drawable);

    struct ntg_measure_output result;
    if(object->__wrapped_measure_fn != NULL)
    {
        result = object->__wrapped_measure_fn(&(object->__drawable),
                orientation, for_size, context);
    }
    else result = (struct ntg_measure_output) {0};

    size_t user_min_size = (orientation == NTG_ORIENTATION_HORIZONTAL) ?
        object->__min_size.x :
        object->__min_size.y;
    size_t user_max_size = (orientation == NTG_ORIENTATION_HORIZONTAL) ?
        object->__max_size.x :
        object->__max_size.y;
    size_t user_grow = (orientation == NTG_ORIENTATION_HORIZONTAL) ?
        object->__grow.x :
        object->__grow.y;

    user_min_size = _min2_size(user_min_size, user_max_size);

    if(user_max_size < result.min_size)
        result.min_size = user_max_size;
    else
        result.min_size = _max2_size(result.min_size, user_min_size);

    if(user_min_size > result.max_size)
        result.max_size = user_min_size;
    else
        result.max_size = _min2_size(result.max_size, user_max_size);

    result.natural_size = _clamp_size(result.min_size,
            result.natural_size, result.max_size);

    result.grow = user_grow;

    return result;
}

static void __ntg_object_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        struct ntg_measure_output measure_output,
        const ntg_constrain_context* context,
        ntg_constrain_output* output)
{
    assert(drawable != NULL);
    assert(context != NULL);
    assert(output != NULL);

    const ntg_object* object = ntg_drawable_user(drawable);

    /* Check if to call wrapped constrain fn */
    if((size == 0) || (object->__wrapped_constrain_fn == NULL))
    {
        const ntg_drawable_vec* children = drawable->_get_children_fn(drawable);
        if(children == NULL) return;

        size_t i;
        struct ntg_constrain_result result = {0};
        for(i = 0; i < children->_count; i++)
        {
            ntg_constrain_output_set(output, children->_data[i], result);
        }
    }
    else
    {
        object->__wrapped_constrain_fn(drawable, orientation, size,
                measure_output, context, output);
    }
}

static void __ntg_object_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* output)
{
    assert(drawable != NULL);
    assert(context != NULL);
    assert(output != NULL);

    const ntg_object* object = ntg_drawable_user(drawable);

    if(ntg_xy_size_is_zero(size) || (object->__wrapped_arrange_fn == NULL))
    {
        const ntg_drawable_vec* children = drawable->_get_children_fn(drawable);
        if(children == NULL) return;

        size_t i;
        struct ntg_arrange_result result = {0};
        for(i = 0; i < children->_count; i++)
        {
            ntg_arrange_output_set(output, children->_data[i], result);
        }
    }
    else
    {
        object->__wrapped_arrange_fn(drawable, size, context, output);
    }
}

static void __ntg_object_draw_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        ntg_drawing* out_drawing)
{
    assert(drawable != NULL);
    assert(out_drawing != NULL);

    if(ntg_xy_size_is_zero(size)) return;

    const ntg_object* object = ntg_drawable_user(drawable);
    // ntg_drawable* drawable = &(object->__drawable);

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = object->__background;
        }
    }

    if(object->__wrapped_draw_fn != NULL)
        object->__wrapped_draw_fn(drawable, size, out_drawing);
}

static bool __ntg_object_process_key_fn(
        ntg_drawable* drawable,
        struct nt_key_event key_event,
        struct ntg_process_key_context context)
{
    assert(drawable != NULL);
    assert(context.scene != NULL);

    ntg_object* object = ntg_drawable_user_(drawable);
    // ntg_drawable* drawable = &(object->__drawable);

    if(object->__wrapped_process_key_fn != NULL)
        return object->__wrapped_process_key_fn(drawable, key_event, context);
    else
        return false;
}

static void __ntg_object_on_focus_fn(
        ntg_drawable* drawable,
        struct ntg_focus_context context)
{
    assert(drawable != NULL);
    assert(context.scene != NULL);

    ntg_object* object = ntg_drawable_user_(drawable);
    // ntg_drawable* drawable = &(object->__drawable);

    if(object->__wrapped_process_key_fn != NULL)
        object->__wrapped_on_focus_fn(drawable, context);
}

static ntg_drawable_vec_view __ntg_object_get_children_fn_(
        ntg_drawable* drawable)
{
    assert(drawable != NULL);

    ntg_object* object = ntg_drawable_user_(drawable);

    ntg_drawable_vec_view view;
    __ntg_drawable_vec_view_init__(&view, object->__children_drawables);

    return view;
}

static const ntg_drawable_vec* __ntg_object_get_children_fn(
        const ntg_drawable* drawable)
{
    assert(drawable != NULL);

    const ntg_object* object = ntg_drawable_user(drawable);

    return object->__children_drawables;
}

static ntg_drawable* __ntg_object_get_parent_fn_(
        ntg_drawable* drawable)
{
    assert(drawable != NULL);

    ntg_object* object = ntg_drawable_user_(drawable);

    return (object->__parent != NULL) ?
        &(object->__parent->__drawable) :
        NULL;
}

static const ntg_drawable* __ntg_object_get_parent_fn(
        const ntg_drawable* drawable)
{
    assert(drawable != NULL);

    const ntg_object* object = ntg_drawable_user(drawable);

    return (object->__parent != NULL) ?
        &(object->__parent->__drawable) :
        NULL;
}
