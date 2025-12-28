#include <stdlib.h>
#include <assert.h>

#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/loop/ntg_loop.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "shared/_ntg_shared.h"

/* ---------------------------------------------------------------- */
/* IDENTITY */
/* ---------------------------------------------------------------- */

bool ntg_object_is_widget(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_entity_instanceof(((const ntg_entity*)object)->_type, &NTG_ENTITY_WIDGET);
}

bool ntg_object_is_decorator(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_entity_instanceof(((const ntg_entity*)object)->_type, &NTG_ENTITY_DECORATOR);
}

bool ntg_object_is_border(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_entity_instanceof(((const ntg_entity*)object)->_type, &NTG_ENTITY_BORDER);
}

bool ntg_object_is_padding(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_entity_instanceof(((const ntg_entity*)object)->_type, &NTG_ENTITY_PADDING);
}

/* ---------------------------------------------------------------- */
/* OBJECT TREE */
/* ---------------------------------------------------------------- */

ntg_scene* ntg_object_get_scene(ntg_object* object)
{
    assert(object != NULL);

    return object->__scene;
}

const ntg_object* ntg_object_get_group_root(const ntg_object* object)
{
    assert(object != NULL);

    const ntg_object* it_obj = object;
    while(true)
    {
        if((it_obj->__parent == NULL) || ntg_object_is_widget(it_obj->__parent))
        {
            return it_obj;
        }

        it_obj = it_obj->__parent;
    }
}

ntg_object* ntg_object_get_group_root_(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;

    while(true)
    {
        if((it_obj->__parent == NULL) || ntg_object_is_widget(it_obj->__parent))
        {
            return it_obj;
        }

        it_obj = it_obj->__parent;
    }
}

const ntg_object* ntg_object_get_root(const ntg_object* object, ntg_object_parent_opts opts)
{
    assert(object != NULL);

    const ntg_object* it_obj = object;
    const ntg_object* prev = NULL;
    while(true)
    {
        prev = it_obj;
        it_obj = ntg_object_get_parent(object, opts);
        
        if(it_obj == NULL) return prev;
    }

    return NULL;
}

ntg_object* ntg_object_get_root_(ntg_object* object, ntg_object_parent_opts opts)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    ntg_object* prev = NULL;
    while(true)
    {
        prev = it_obj;
        it_obj = ntg_object_get_parent_(object, opts);
        
        if(it_obj == NULL) return prev;
    }

    return NULL;
}

const ntg_object* ntg_object_get_parent(const ntg_object* object, ntg_object_parent_opts opts)
{
    assert(object != NULL);

    const ntg_object* group_root;
    switch(opts)
    {
        case NTG_OBJECT_PARENT_INCL_DECOR:
            return object->__parent;
        case NTG_OBJECT_PARENT_EXCL_DECOR:
             group_root = ntg_object_get_group_root(object);
             return group_root->__parent;
        default: assert(0);
    }
}

ntg_object* ntg_object_get_parent_(ntg_object* object, ntg_object_parent_opts opts)
{
    assert(object != NULL);

    ntg_object* group_root;
    switch(opts)
    {
        case NTG_OBJECT_PARENT_INCL_DECOR:
            return object->__parent;
        case NTG_OBJECT_PARENT_EXCL_DECOR:
             group_root = ntg_object_get_group_root_(object);
             return group_root->__parent;
        default: assert(0);
    }
}

const ntg_object* ntg_object_get_base_widget(const ntg_object* object)
{
    assert(object != NULL);

    const ntg_object* it_obj = object;
    while(ntg_object_is_decorator(it_obj))
        it_obj = it_obj->__children->_data[0];

    return it_obj;
}

ntg_object* ntg_object_get_base_widget_(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    while(ntg_object_is_decorator(it_obj))
        it_obj = it_obj->__children->_data[0];

    return it_obj;
}

struct ntg_object_vecv ntg_object_get_children_(ntg_object* object)
{
    assert(object != NULL);

    return ntg_object_vecv_new(object->__children);
}

struct ntg_const_object_vecv ntg_object_get_children(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_const_object_vecv_new_(object->__children);
}

bool ntg_object_is_ancestor(const ntg_object* object, const ntg_object* ancestor)
{
    assert(object != NULL);
    assert(ancestor != NULL);

    const ntg_object* it_object = object;
    while(it_object != NULL)
    {
        if(it_object == ancestor) return true;

        it_object = it_object->__parent;
    }
    
    return false;
}

bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant)
{
    assert(object != NULL);
    assert(descendant != NULL);

    return ntg_object_is_ancestor(descendant, object);
}

void ntg_object_tree_perform(ntg_object* object,
        ntg_object_perform_mode mode,
        void (*perform_fn)(ntg_object* object, void* data),
        void* data)
{
    assert(object != NULL);
    assert(perform_fn != NULL);

    ntg_object_vec* children = object->__children;
    size_t count = children->_count;

    ntg_object* it_child;
    size_t i;
    if(mode == NTG_OBJECT_PERFORM_TOP_DOWN)
    {
        perform_fn(object, data);

        for(i = 0; i < count; i++)
        {
            it_child = children->_data[i];
            ntg_object_tree_perform(it_child, mode, perform_fn, data);
        }
    }
    else
    {
        for(i = 0; i < count; i++)
        {
            it_child = children->_data[i];
            ntg_object_tree_perform(it_child, mode, perform_fn, data);
        }

        perform_fn(object, data);
    }
}

/* ---------------------------------------------------------------- */
/* SIZE CONTROL */
/* ---------------------------------------------------------------- */

struct ntg_xy ntg_object_get_min_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__min_size;
}

void ntg_object_set_min_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__min_size = size;
}

struct ntg_xy ntg_object_get_max_size(ntg_object* object)
{
    assert(object != NULL);

    return object->__max_size;
}

void ntg_object_set_max_size(ntg_object* object, struct ntg_xy size)
{
    assert(object != NULL);

    object->__max_size = size;
}

struct ntg_xy ntg_object_get_grow(const ntg_object* object)
{
    assert(object != NULL);
    
    return object->__grow;
}

void ntg_object_set_grow(ntg_object* object, struct ntg_xy grow)
{
    assert(object != NULL);

    object->__grow = grow;
}

/* ---------------------------------------------------------------- */
/* PADDING & BORDER */
/* ---------------------------------------------------------------- */

/* Assumes that widget exists in the object group */
static void separate_object_group(ntg_object* group_root, ntg_object** border,
        ntg_object** padding, ntg_object** widget)
{
    assert(group_root != NULL);
    assert(border != NULL);
    assert(padding != NULL);
    assert(widget != NULL);

    ntg_object* _border;
    ntg_object* _padding;
    ntg_object* _widget;

    if(ntg_object_is_border(group_root))
    {
        _border = group_root;

        if(!ntg_object_is_widget(group_root->__children->_data[0])) // padding
        {
            _padding = _border->__children->_data[0];
            _widget = _padding->__children->_data[0];

            _ntg_object_rm_child(_border, _padding);
            _ntg_object_rm_child(_padding, _widget);
        }
        else // widget
        {
            _padding = NULL;
            _widget = _border->__children->_data[0];

            _ntg_object_rm_child(_border, _widget);
        }
    }
    else if(ntg_object_is_padding(group_root))
    {
        _border = NULL;
        _padding = group_root;
        _widget = _padding->__children->_data[0];

        _ntg_object_rm_child(_padding, _widget);
    }
    else // group_root->__type is widget
    {
        _border = NULL;
        _padding = NULL;
        _widget = group_root;
    }

    (*border) = _border;
    (*padding) = _padding;
    (*widget) = _widget;
}

// returns group root
static ntg_object* join_object_group(ntg_object* border,
        ntg_object* padding, ntg_object* widget)
{
    assert(widget != NULL);

    if(border != NULL)
    {
        if(padding != NULL)
        {
            _ntg_object_add_child(border, padding);
            _ntg_object_add_child(padding, widget);
            return border;
        }
        else // border == NULL
        {
            _ntg_object_add_child(border, widget);
            return border;
        }
    }
    else
    {
        if(padding != NULL)
        {
            _ntg_object_add_child(padding, widget);
            return padding;
        }
        else // padding == NULL
        {
            return widget;
        }
    }
}

const ntg_padding* ntg_object_get_padding(const ntg_object* object)
{
    assert(object != NULL);

    const ntg_object* parent = ntg_object_get_parent(object, NTG_OBJECT_PARENT_INCL_DECOR);
    if((parent == NULL) || (!ntg_object_is_padding(parent)))
        return NULL;
    else return (ntg_padding*)parent;
}

ntg_padding* ntg_object_get_padding_(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* parent = ntg_object_get_parent_(object, NTG_OBJECT_PARENT_INCL_DECOR);
    if((parent == NULL) || (!ntg_object_is_padding(parent)))
        return NULL;
    else return (ntg_padding*)parent;
}

ntg_padding* ntg_object_set_padding(ntg_object* object, ntg_padding* padding)
{
    assert(object != NULL);
    assert(padding != ntg_object_get_border(object));

    ntg_object* root = ntg_object_get_group_root_(object);

    ntg_object* root_parent = root->__parent;
    if(root_parent != NULL)
        _ntg_object_rm_child(root_parent, root);

    ntg_object *_border, *_padding, *_widget;
    separate_object_group(root, &_border, &_padding, &_widget);
    ntg_object* new_root = join_object_group(_border, (ntg_object*)padding, _widget);

    if(root_parent != NULL)
        _ntg_object_add_child(root_parent, new_root);

    return (ntg_padding*)_padding;
}

const ntg_padding* ntg_object_get_border(const ntg_object* object)
{
    assert(object != NULL);

    const ntg_object* root = ntg_object_get_group_root(object);
    if(!ntg_object_is_border(root)) return NULL;
    else return (ntg_padding*)root;
}

ntg_padding* ntg_object_get_border_(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* root = ntg_object_get_group_root_(object);
    if(!ntg_object_is_border(root)) return NULL;
    else return (ntg_padding*)root;
}

ntg_padding* ntg_object_set_border(ntg_object* object, ntg_padding* border)
{
    assert(object != NULL);
    assert(border != ntg_object_get_padding(object));

    ntg_object* root = ntg_object_get_group_root_(object);

    ntg_object* root_parent = root->__parent;
    if(root_parent != NULL)
        _ntg_object_rm_child(root_parent, root);

    ntg_object *_border, *_padding, *_widget;
    separate_object_group(root, &_border, &_padding, &_widget);
    ntg_object* new_root = join_object_group((ntg_object*)border, _padding, _widget);

    if(root_parent != NULL)
        _ntg_object_add_child(root_parent, new_root);

    return (ntg_padding*)_border;
}

ntg_cell ntg_object_get_bg(const ntg_object* object)
{
    assert(object != NULL);

    return object->__background;
}

void ntg_object_set_bg(ntg_object* object, ntg_cell background)
{
    assert(object != NULL);

    object->__background = background;
}

/* ---------------------------------------------------------------- */
/* LAYOUT */
/* ---------------------------------------------------------------- */

void* ntg_object_layout_init(const ntg_object* object)
{
    assert(object != NULL);

    return (object->__layout_init_fn != NULL) ?
        object->__layout_init_fn(object) : NULL;
}

void ntg_object_layout_deinit(const ntg_object* object, void* layout_data)
{
    assert(object != NULL);

    if(object->__layout_deinit_fn != NULL)
        object->__layout_deinit_fn(object, layout_data);
}

struct ntg_object_measure ntg_object_measure(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena)
{
    assert(object != NULL);
    assert(out != NULL);
    assert(arena != NULL);

    size_t user_grow = (orientation == NTG_ORIENTATION_H) ?
        object->__grow.x :
        object->__grow.y;

    if(for_size == 0) return (struct ntg_object_measure) {
        .min_size = 0,
        .natural_size = 0,
        .max_size = 0,
        .grow = user_grow
    };

    struct ntg_object_measure result;
    if(object->__measure_fn != NULL)
    {
        result = object->__measure_fn(object, orientation, for_size,
                ctx, out, layout_data, arena);
    }
    else result = (struct ntg_object_measure) {0};

    size_t user_min_size = (orientation == NTG_ORIENTATION_H) ?
        object->__min_size.x :
        object->__min_size.y;
    size_t user_max_size = (orientation == NTG_ORIENTATION_H) ?
        object->__max_size.x :
        object->__max_size.y;

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

void ntg_object_constrain(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* layout_data,
        sarena* arena)
{
    assert(object != NULL);
    assert(out != NULL);
    assert(arena != NULL);

    /* Check if to call wrapped constrain fn */
    if((size == 0) || (object->__constrain_fn == NULL))
    {
        const ntg_object_vec* children = object->__children;
        if(children == NULL) return;

        size_t i;
        for(i = 0; i < children->_count; i++)
            ntg_object_size_map_set(out->sizes, children->_data[i], 0);
    }
    else
    {
        object->__constrain_fn(object, orientation, size,
                ctx, out, layout_data, arena);
    }
}

void ntg_object_arrange(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* layout_data,
        sarena* arena)
{
    assert(object != NULL);
    assert(out != NULL);
    assert(arena != NULL);

    if(ntg_xy_size_is_zero(size) || (object->__arrange_fn == NULL))
    {
        const ntg_object_vec* children = object->__children;
        if(children == NULL) return;

        size_t i;
        for(i = 0; i < children->_count; i++)
            ntg_object_xy_map_set(out->pos, children->_data[i], ntg_xy(0, 0));
    }
    else
    {
        object->__arrange_fn(object, size, ctx, out, layout_data, arena);
    }
}

void ntg_object_draw(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena)
{
    assert(object != NULL);
    assert(out != NULL);
    assert(arena != NULL);
    if(ntg_xy_size_is_zero(size)) return;

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(out->drawing, ntg_xy(j, i));
            (*it_cell) = object->__background;
        }
    }

    if(object->__draw_fn != NULL)
        object->__draw_fn(object, size, ctx, out, layout_data, arena);
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_object_feed_event(
        ntg_object* object,
        struct ntg_loop_event event,
        ntg_loop_ctx* ctx)
{
    assert(object != NULL);

    if(object->__event_fn != NULL)
        return object->__event_fn(object, event, ctx);
    else
        return false;
}

void ntg_object_set_event_fn(
        ntg_object* object,
        ntg_object_event_fn fn)
{
    assert(object != NULL);

    object->__event_fn = fn;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_object* object);

void _ntg_object_init_(ntg_object* object, struct ntg_object_layout_ops object_data)
{
    assert(object != NULL);

    __init_default_values(object);

    object->__children = ntg_object_vec_new();
    object->__background = ntg_cell_default();
    object->__scene = NULL;
    object->__layout_init_fn = object_data.layout_init_fn;
    object->__layout_deinit_fn = object_data.layout_deinit_fn;
    object->__measure_fn = object_data.measure_fn;
    object->__constrain_fn = object_data.constrain_fn;
    object->__arrange_fn = object_data.arrange_fn;
    object->__draw_fn = object_data.draw_fn;
    object->__event_fn = NULL;
}

static void __init_default_values(ntg_object* object)
{
    object->__parent = NULL;
    object->__children = NULL;
    object->__scene = NULL;

    object->__min_size = ntg_xy(0, 0);
    object->__max_size = ntg_xy(NTG_SIZE_MAX, NTG_SIZE_MAX);
    object->__grow = ntg_xy(1, 1);

    object->__layout_init_fn = NULL;
    object->__layout_deinit_fn = NULL;
    object->__measure_fn = NULL;
    object->__constrain_fn = NULL;
    object->__arrange_fn = NULL;
    object->__draw_fn = NULL;
}

void _ntg_object_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_object* object = (ntg_object*)entity;
    ntg_object_vec_destroy(object->__children);

    __init_default_values(object);
}

void _ntg_object_set_scene(ntg_object* object, ntg_scene* scene)
{
    assert(object != NULL);

    if(object->__scene == scene) return;

    object->__scene = scene;
}

void _ntg_object_add_child(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    assert(child->__parent == NULL);

    // ntg_scene performs this
    // ntg_object_tree_perform(child, NTG_OBJECT_PERFORM_TOP_DOWN,
    //         set_scene_fn, object->__scene);

    ntg_object_vec_append(object->__children, child);

    struct ntg_event_object_chldadd_data data = {
        .child = child
    };

    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_CHLDADD, &data);

    child->__parent = object;
}

void _ntg_object_rm_child(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    if(child->__parent != object) return;

    // ntg_scene performs this
    // ntg_object_tree_perform(child, NTG_OBJECT_PERFORM_TOP_DOWN,
    //         set_scene_fn, NULL);

    ntg_object_vec_remove(object->__children, child);

    struct ntg_event_object_chldrm_data data = {
        .child = child
    };

    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_CHLDRM, &data);

    child->__parent = NULL;
}

void _ntg_object_rm_children(ntg_object* object)
{
    assert(object != NULL);

    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t count = children.count;

    size_t i;

    ntg_object** children_cpy = (ntg_object**)malloc(sizeof(ntg_object*) * count);
    for(i = 0; i < count; i++)
        children_cpy[i] = children.data[i];

    for(i = 0; i < count; i++)
        _ntg_object_rm_child(object, children_cpy[i]);

    free(children_cpy);
}
