#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <stdlib.h>

// TODO: validate?
static void _ntg_object_add_child_dcr(ntg_object* object, ntg_object* child);
static void _ntg_object_rm_child_dcr(ntg_object* object, ntg_object* child);
// static void _ntg_object_rm_children(ntg_object* object);

/* ---------------------------------------------------------------- */
/* IDENTITY */
/* ---------------------------------------------------------------- */

bool ntg_object_is_widget(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_entity_instance_of(((const ntg_entity*)object)->_type, &NTG_ENTITY_WIDGET);
}

bool ntg_object_is_decorator(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_entity_instance_of(((const ntg_entity*)object)->_type, &NTG_ENTITY_DECORATOR);
}

bool ntg_object_is_border(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_entity_instance_of(((const ntg_entity*)object)->_type, &NTG_ENTITY_BORDER);
}

bool ntg_object_is_padding(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_entity_instance_of(((const ntg_entity*)object)->_type, &NTG_ENTITY_PADDING);
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

const ntg_object* ntg_object_get_root(const ntg_object* object, bool incl_decor)
{
    assert(object != NULL);

    const ntg_object* it_obj = object;
    const ntg_object* prev = NULL;
    while(true)
    {
        prev = it_obj;
        it_obj = ntg_object_get_parent(object, incl_decor);
        
        if(it_obj == NULL) return prev;
    }

    return NULL;
}

ntg_object* ntg_object_get_root_(ntg_object* object, bool incl_decor)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    ntg_object* prev = NULL;
    while(true)
    {
        prev = it_obj;
        it_obj = ntg_object_get_parent_(object, incl_decor);
        
        if(it_obj == NULL) return prev;
    }

    return NULL;
}

const ntg_object* ntg_object_get_parent(const ntg_object* object, bool incl_decor)
{
    assert(object != NULL);

    const ntg_object* group_root;
    if(incl_decor)
        return object->__parent;
    else
    {
         group_root = ntg_object_get_group_root(object);
         return group_root->__parent;
    }
}

ntg_object* ntg_object_get_parent_(ntg_object* object, bool incl_decor)
{
    assert(object != NULL);

    ntg_object* group_root;
    if(incl_decor)
        return object->__parent;
    else
    {
        group_root = ntg_object_get_group_root_(object);
        return group_root->__parent;
    }
}

const ntg_object* ntg_object_get_base_widget(const ntg_object* object)
{
    assert(object != NULL);

    const ntg_object* it_obj = object;
    while(ntg_object_is_decorator(it_obj))
        it_obj = it_obj->__children._data[0];

    return it_obj;
}

ntg_object* ntg_object_get_base_widget_(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* it_obj = object;
    while(ntg_object_is_decorator(it_obj))
        it_obj = it_obj->__children._data[0];

    return it_obj;
}

struct ntg_object_vecv ntg_object_get_children_(ntg_object* object)
{
    assert(object != NULL);

    return ntg_object_vecv_new(&object->__children);
}

struct ntg_const_object_vecv ntg_object_get_children(const ntg_object* object)
{
    assert(object != NULL);

    return ntg_const_object_vecv_new_(&object->__children);
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

    _ntg_object_mark_change(object);
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

    _ntg_object_mark_change(object);
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

    _ntg_object_mark_change(object);
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

        if(!ntg_object_is_widget(group_root->__children._data[0])) // padding
        {
            _padding = _border->__children._data[0];
            _widget = _padding->__children._data[0];

            _ntg_object_rm_child_dcr(_border, _padding);
            _ntg_object_rm_child_dcr(_padding, _widget);
        }
        else // widget
        {
            _padding = NULL;
            _widget = _border->__children._data[0];

            _ntg_object_rm_child_dcr(_border, _widget);
        }
    }
    else if(ntg_object_is_padding(group_root))
    {
        _border = NULL;
        _padding = group_root;
        _widget = _padding->__children._data[0];

        _ntg_object_rm_child_dcr(_padding, _widget);
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
            _ntg_object_add_child_dcr(border, padding);
            _ntg_object_add_child_dcr(padding, widget);
            return border;
        }
        else // border == NULL
        {
            _ntg_object_add_child_dcr(border, widget);
            return border;
        }
    }
    else
    {
        if(padding != NULL)
        {
            _ntg_object_add_child_dcr(padding, widget);
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

    const ntg_object* parent = ntg_object_get_parent(object, true);
    if((parent == NULL) || (!ntg_object_is_padding(parent)))
        return NULL;
    else return (ntg_padding*)parent;
}

ntg_padding* ntg_object_get_padding_(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* parent = ntg_object_get_parent_(object, true);
    if((parent == NULL) || (!ntg_object_is_padding(parent)))
        return NULL;
    else return (ntg_padding*)parent;
}

void ntg_object_set_padding(ntg_object* object, ntg_padding* padding)
{
    assert(object != NULL);
    assert((padding == NULL) || (padding != ntg_object_get_border(object)));
    if(padding == ntg_object_get_padding(object)) return;

    ntg_object* root = ntg_object_get_group_root_(object);

    ntg_object* root_parent = root->__parent;
    if(root_parent != NULL)
        _ntg_object_rm_child_dcr(root_parent, root);

    ntg_object *_border, *_padding, *_widget;
    separate_object_group(root, &_border, &_padding, &_widget);
    ntg_object* new_root = join_object_group(_border, (ntg_object*)padding, _widget);

    if(root_parent != NULL)
        _ntg_object_add_child_dcr(root_parent, new_root);

    struct ntg_event_object_padchng data = {
        .old = (ntg_padding*)_padding,
        .new = padding
    };
    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_PADCHNG, &data);
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

void ntg_object_set_border(ntg_object* object, ntg_padding* border)
{
    assert(object != NULL);
    assert((border == NULL) || (border != ntg_object_get_padding(object)));
    if(border == ntg_object_get_border(object)) return;

    ntg_object* root = ntg_object_get_group_root_(object);

    ntg_object* root_parent = root->__parent;
    if(root_parent != NULL)
        _ntg_object_rm_child_dcr(root_parent, root);

    ntg_object *_border, *_padding, *_widget;
    separate_object_group(root, &_border, &_padding, &_widget);
    ntg_object* new_root = join_object_group((ntg_object*)border, _padding, _widget);

    if(root_parent != NULL)
        _ntg_object_add_child_dcr(root_parent, new_root);

    struct ntg_event_object_bordchng data = {
        .old = (ntg_padding*)_border,
        .new = border
    };
    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_BORDCHNG, &data);
}

void ntg_object_set_background(ntg_object* object, struct ntg_vcell background)
{
    assert(object != NULL);

    object->_background = background;
    _ntg_object_mark_change(object);
}

/* ---------------------------------------------------------------- */
/* LAYOUT */
/* ---------------------------------------------------------------- */

void* ntg_object_layout_init(const ntg_object* object)
{
    assert(object != NULL);

    return (object->__layout_ops.init_fn != NULL) ?
        object->__layout_ops.init_fn(object) : NULL;
}

void ntg_object_layout_deinit(const ntg_object* object, void* layout_data)
{
    assert(object != NULL);

    if(object->__layout_ops.deinit_fn != NULL)
        object->__layout_ops.deinit_fn(object, layout_data);
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

    size_t user_grow = (orientation == NTG_ORIENT_H) ?
        object->__grow.x :
        object->__grow.y;

    if(for_size == 0) return (struct ntg_object_measure) {
        .min_size = 0,
        .natural_size = 0,
        .max_size = 0,
        .grow = user_grow
    };

    struct ntg_object_measure result;
    if(object->__layout_ops.measure_fn != NULL)
    {
        result = object->__layout_ops.measure_fn(object, orientation, for_size,
                ctx, out, layout_data, arena);
    }
    else result = (struct ntg_object_measure) {0};

    size_t user_min_size = (orientation == NTG_ORIENT_H) ?
        object->__min_size.x :
        object->__min_size.y;
    size_t user_max_size = (orientation == NTG_ORIENT_H) ?
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
    if((size == 0) || (object->__layout_ops.constrain_fn == NULL))
    {
        const ntg_object_vec* children = &object->__children;

        size_t i;
        for(i = 0; i < children->_count; i++)
            ntg_object_size_map_set(out->sizes, children->_data[i], 0);
    }
    else
    {
        object->__layout_ops.constrain_fn(object, orientation, size,
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

    if(ntg_xy_size_is_zero(size) || (object->__layout_ops.arrange_fn == NULL))
    {
        const ntg_object_vec* children = &object->__children;

        size_t i;
        for(i = 0; i < children->_count; i++)
            ntg_object_xy_map_set(out->positions, children->_data[i], ntg_xy(0, 0));
    }
    else
    {
        object->__layout_ops.arrange_fn(object, size, ctx, out, layout_data, arena);
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
    struct ntg_vcell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(out->drawing, ntg_xy(j, i));
            (*it_cell) = object->_background;
        }
    }

    if(object->__layout_ops.draw_fn != NULL)
        object->__layout_ops.draw_fn(object, size, ctx, out, layout_data, arena);
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_object_feed_event(
        ntg_object* object,
        struct nt_event event,
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
/* PROTECTED */
/* -------------------------------------------------------------------------- */

static void _init_default_values(ntg_object* object);

void _ntg_object_init(ntg_object* object, struct ntg_object_layout_ops layout_ops)
{
    assert(object != NULL);

    _init_default_values(object);

    ntg_object_vec_init(&object->__children);
    object->_background = ntg_vcell_default();
    object->__scene = NULL;
    object->__layout_ops = layout_ops;
    object->__event_fn = NULL;
}

static void _init_default_values(ntg_object* object)
{
    object->__parent = NULL;
    object->__children = (ntg_object_vec) {0};
    object->__scene = NULL;

    object->__min_size = ntg_xy(0, 0);
    object->__max_size = ntg_xy(NTG_SIZE_MAX, NTG_SIZE_MAX);
    object->__grow = ntg_xy(1, 1);

    object->__layout_ops = (struct ntg_object_layout_ops) {0};
}

void _ntg_object_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_object* object = (ntg_object*)entity;
    ntg_object_vec_init(&object->__children);

    _init_default_values(object);
}

void _ntg_object_set_scene(ntg_object* object, ntg_scene* scene)
{
    assert(object != NULL);

    if(object->__scene == scene) return;

    object->__scene = scene;
}

bool _ntg_object_validate_add_child(ntg_object* object, ntg_object* child)
{
    if((object == NULL) || (child == NULL)) return false;

    if(!ntg_object_is_widget(object)) return false;
    if(!ntg_object_is_widget(child)) return false;

    ntg_object* child_parent = ntg_object_get_parent_(child, false);
    if(child_parent != NULL) return false;

    if((child->__scene != NULL) && (child->__scene->_root == child))
        return false;

    return true;
}

void _ntg_object_add_child(ntg_object* object, ntg_object* child)
{
    ntg_object* child_root = ntg_object_get_group_root_(child);

    _ntg_object_add_child_dcr(object, child_root);

    struct ntg_event_object_chldadd_data data = { .child = child };
    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_CHLDADD, &data);
}

void _ntg_object_rm_child(ntg_object* object, ntg_object* child)
{
    ntg_object* child_root = ntg_object_get_group_root_(child);
    assert(ntg_object_vec_has(&object->__children, child_root));

    _ntg_object_rm_child_dcr(object, child_root);

    child->__parent = object;

    struct ntg_event_object_chldrm_data data = { .child = child };
    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_CHLDRM, &data);
}

void _ntg_object_mark_change(ntg_object* object)
{
    ntg_entity_raise_event((ntg_entity*)object, NULL, NTG_EVENT_OBJECT_DIFF, NULL);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

static void _ntg_object_add_child_dcr(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    ntg_object* child_parent = ntg_object_get_parent_(child, true);
    assert(child_parent == NULL);

    ntg_object_vec_add(&object->__children, child);

    child->__parent = object;

    struct ntg_event_object_chldadd_dcr_data data = { .child = child };
    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_CHLDADD_DCR, &data);

    _ntg_object_mark_change(object);
}

static void _ntg_object_rm_child_dcr(ntg_object* object, ntg_object* child)
{
    assert(object != NULL);
    assert(child != NULL);

    ntg_object* child_parent = ntg_object_get_parent_(
            child, true);
    assert(child_parent != NULL);

    ntg_object_vec_rm(&object->__children, child);

    child->__parent = NULL;

    struct ntg_event_object_chldrm_dcr_data data = { .child = child };
    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_CHLDRM_DCR, &data);

    _ntg_object_mark_change(object);
}

// static void _ntg_object_rm_children(ntg_object* object)
// {
//     assert(object != NULL);
//
//     struct ntg_object_vecv children = ntg_object_get_children_(object);
//     size_t count = children.count;
//
//     size_t i;
//
//     ntg_object** children_cpy = (ntg_object**)malloc(sizeof(ntg_object*) * count);
//     for(i = 0; i < count; i++)
//         children_cpy[i] = children.data[i];
//
//     for(i = 0; i < count; i++)
//         _ntg_object_rm_child_dcr(object, children_cpy[i]);
//
//     free(children_cpy);
// }
