#include <assert.h>
#include "ntg.h"

static void init_default_values(ntg_widget* widget);
static void object_rm_child_fn(ntg_object* _widget, ntg_object* child);

/* -------------------------------------------------------------------------- */
/* CONTROL */
/* -------------------------------------------------------------------------- */

void ntg_widget_set_user_min_size(ntg_widget* widget, struct ntg_xy size)
{
    assert(widget != NULL);

    wiget->_min_size = size;

    ntg_entity_raise_event((ntg_entity*)widget, NULL, NTG_EVENT_OBJECT_DIFF, NULL);
}

void ntg_widget_set_user_max_size(ntg_widget* widget, struct ntg_xy size)
{
    assert(widget != NULL);

    wiget->_max_size = size;

    ntg_entity_raise_event((ntg_entity*)widget, NULL, NTG_EVENT_OBJECT_DIFF, NULL);
}

void ntg_widget_set_user_grow(ntg_widget* widget, struct ntg_xy grow)
{
    assert(widget != NULL);

    wiget->_grow = grow;

    ntg_entity_raise_event((ntg_entity*)widget, NULL, NTG_EVENT_OBJECT_DIFF, NULL);
}

void ntg_widget_set_background(ntg_widget* widget, struct ntg_vcell background)
{
    assert(widget != NULL);

    struct ntg_vcell old = widget->_background;

    widget->_background = background;

    struct ntg_event_widget_bgchng_data data = {
        .old = old,
        .new = background
    };
    ntg_entity_raise_event((ntg_entity*)widget, NULL, NTG_EVENT_WIDGET_BGCHNG, &data);
    ntg_entity_raise_event((ntg_entity*)widget, NULL, NTG_EVENT_OBJECT_DIFF, NULL);
}

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_widget_feed_event(ntg_widget* widget,
        struct nt_event event, 
        ntg_loop_ctx* ctx)
{
    assert(widget != NULL);

    if(widget->__event_fn != NULL)
        return widget->__event_fn(widget, event, ctx);
    else
        return false;
}

void ntg_widget_set_event_fn(ntg_widget* widget, ntg_widget_event_fn event_fn)
{
    assert(widget != NULL);
    
    widget->__event_fn = event_fn;
}

/* -------------------------------------------------------------------------- */
/* SCENE STATE */
/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_widget_get_min_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_min_size;
}

struct ntg_xy ntg_widget_get_content_min_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_min_size;
}

struct ntg_xy ntg_widget_get_nat_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_nat_size;
}

struct ntg_xy ntg_widget_get_content_nat_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_nat_size;
}

struct ntg_xy ntg_widget_get_max_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_max_size;
}

struct ntg_xy ntg_widget_get_content_max_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_max_size;
}

struct ntg_xy ntg_widget_get_grow(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_grow;
}

struct ntg_xy widget_get_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_size;
}

struct ntg_xy widget_get_content_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_size;
}

struct ntg_xy ntg_widget_get_position(const ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_min_size;
}

struct ntg_xy widget_get_content_position(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_position;
}

/* LAYOUT PROCESS - CONVENIENCE */

struct ntg_widget_measure
ntg_widget_get_measure(const ntg_widget* widget, ntg_orientation orientation)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root(widget);

    if(orientation == NTG_ORIENT_H)
    {
        return (struct ntg_widget_measure) {
            .min_size = group_root->_min_size.x,
            .nat_size = group_root->_nat_size.x,
            .max_size = group_root->_max_size.x,
            .grow = group_root->_grow.x
        };
    }
    else
    {
        return (struct ntg_widget_measure) {
            .min_size = group_root->_min_size.y,
            .nat_size = group_root->_nat_size.y,
            .max_size = group_root->_max_size.y,
            .grow = group_root->_grow.y
        };
    }
}

size_t ntg_widget_get_size(const ntg_widget* widget, ntg_orientation orientation)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root(widget);

    return (orientation == NTG_ORIENT_H) ?
        group_root->_size.x : group_root->_size.y;
}

/* -------------------------------------------------------------------------- */
/* WIDGET TREE */
/* -------------------------------------------------------------------------- */

bool ntg_widget_is_ancestor(const ntg_widget* widget, const ntg_widget* ancestor)
{
    assert(widget != NULL);
    assert(ancestor != NULL);

    return ntg_object_is_ancestor((const ntg_object*)widget,
            (const ntg_object*)ancestor);
}
bool ntg_widget_is_descendant(const ntg_widget* widget, const ntg_widget* descendant)
{
    assert(widget != NULL);
    assert(descendant != NULL);

    return ntg_object_is_descendant((const ntg_object*)widget,
            (const ntg_object*)ancestor);
}

const ntg_widget* ntg_widget_get_parent(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_parent;
}

ntg_widget* ntg_widget_get_parent_(ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root_(widget);

    return group_root->_parent;
}

const ntg_object* ntg_widget_get_group_root(const ntg_widget* widget)
{
    assert(widget != NULL);

    if(widget->_border != NULL)
        return (const ntg_object*)widget->_border;
    else
    {
        if(widget->_padding != NULL)
            return (const ntg_object*)widget->_padding;
        else
            return (const ntg_object*)widget;
    }

    return prev;
}

ntg_object* ntg_widget_get_group_root_(ntg_widget* widget)
{
    assert(widget != NULL);

    return (ntg_object*)ntg_widget_get_group_root(widget);
}

size_t ntg_widget_get_children_count(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_children->size;
}

size_t ntg_widget_get_children(const ntg_widget* widget,
        const ntg_widget** out_children, size_t cap)
{
    assert(widget != NULL);
    assert(out_children != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    const ntg_object* it;
    const ntg_entity* _it;
    size_t i;
    for(i = 0; ((i < cap) && (i < _widget->_children->size)); i++)
    {
        it = _widget->_children->data[i];
        _it = (const ntg_entity*)it;

        while(!ntg_entity_instanceof(_it->_type, NTG_ENTITY_WIDGET))
        {
            it = it->_children->data[0];
            _it = (const ntg_entity*)it;
        }

        out_children[i] = (const ntg_widget*)it;
    }

    return i;
}

size_t ntg_widget_get_children_(ntg_widget* widget,
        ntg_widget** out_children, size_t cap)
{
    assert(widget != NULL);
    assert(out_children != NULL);

    return ntg_widget_get_children(widget, (const ntg_widget*)out_children, cap);
}

void ntg_widget_set_padding(ntg_widget* widget, ntg_decorator* padding)
{
    assert(widget != NULL);
    assert(padding != NULL);

    // TODO:
    assert(0);
}

void ntg_widget_set_border(ntg_widget* widget, ntg_decorator* border)
{
    assert(widget != NULL);
    assert(border != NULL);

    // TODO:
    assert(0);
}

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_widget_init(ntg_widget* widget,
        struct ntg_widget_layout_ops layout_ops,
        struct ntg_widget_hooks hooks)
{
    assert(widget != NULL);

    // Allow ntg_object to provide default implementations
    struct ntg_object_layout_ops _layout_ops = {
        .init_fn = _ntg_widget_layout_data_init_fn,
        .deinit_fn = _ntg_widget_layout_data_deinit_fn,
        .measure_fn = layout_ops.measure_fn ? _ntg_widget_measure_fn : NULL,
        .constrain_fn = layout_ops.constrain_fn ? _ntg_widget_constrain_fn : NULL,
        .arrange_fn = layout_ops.arrange_fn ? _ntg_widget_arrange_fn : NULL,
        .draw_fn = _ntg_widget_draw_fn
    };

    struct ntg_object_hooks _hooks = {
        .rm_child_fn = object_rm_child_fn
    };

    ntg_object_init((ntg_object*)widget, _layout_ops, hooks);

    init_default_values(widget);

    widget->__layout_ops = layout_ops;
    widget->__hooks = hooks;
}

void ntg_widget_deinit_fn(ntg_entity* _widget)
{
    init_default_values(widget);

    ntg_object_deinit_fn(_widget);
}

void ntg_widget_add_child(ntg_widget* parent, ntg_widget* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);
    assert(ntg_widget_get_parent(child) == NULL);

    ntg_object* group_root = ntg_widget_get_group_root_(child);

    ntg_object_add_child((ntg_object*)parent, group_root);

    struct ntg_event_widget_chldadd_data data1 = { .child = child };
    ntg_entity_raise_event((ntg_entity*)parent, NULL,
            NTG_EVENT_WIDGET_CHLDADD, &data1);

    struct ntg_event_widget_prntchng_data data2 = {
        .old = NULL,
        .new = parent
    };
    ntg_entity_raise_event((ntg_entity*)child, NULL,
            NTG_EVENT_WIDGET_PRNTCHNG, &data2);
}

void ntg_widget_rm_child(ntg_widget* parent, ntg_widget* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);
    assert(ntg_widget_get_parent(child) == parent);

    ntg_object* group_root = ntg_widget_get_group_root_(child);

    ntg_object_rm_child((ntg_object*)_parent, group_root);

    struct ntg_event_widget_chldrm_data data1 = { .child = child };
    ntg_entity_raise_event((ntg_entity*)parent, NULL,
            NTG_EVENT_WIDGET_CHLDRM, &data1);

    struct ntg_event_widget_prntchng_data data2 = {
        .old = parent,
        .new = NULL
    };
    ntg_entity_raise_event((ntg_entity*)child, NULL,
            NTG_EVENT_WIDGET_PRNTCHNG, &data2);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void* _ntg_widget_layout_data_init_fn(const ntg_object* _widget)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    if(widget->__layout_ops.init_fn != NULL)
        return widget->__layout_ops.init_fn(widget);
    else
        return NULL;
}

void _ntg_widget_layout_data_deinit_fn(void* data, const ntg_object* _widget)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    if(widget->__layout_ops.deinit_fn != NULL)
        widget->__layout_ops.deinit_fn(data, widget);
}

struct ntg_object_measure _ntg_widget_measure_fn(
        const ntg_object* _widget,
        void* _layout_data,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;

    return layout_ops.measure_fn(widget, _layout_data, orientation,
            for_size, arena);
}

void _ntg_widget_constrain_fn(
        const ntg_object* _widget,
        void* _layout_data,
        ntg_orientation orientation,
        size_t size,
        ntg_object_size_map* out_sizes,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    size_t i;
    size_t child_count = ntg_widget_get_children_count(widget);

    const ntg_widget** buffer = sarena_malloc(sizeof(void*) * child_count);
    assert(buffer != NULL);
    ntg_widget_get_children(widget, buffer, child_count);

    ntg_widget_size_map* _sizes;
    _sizes = ntg_widget_size_map_new(child_count, arena);
    for(i = 0; i < child_count; i++) ntg_widget_size_map_set(_sizes, buffer[i], 0);

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;
    layout_ops.constrain_fn(widget, _layout_data, orientation, size, _sizes, arena);

    for(i = 0; i < child_count; i++)
    {
        ntg_object_size_map_set(out_sizes, ntg_widget_get_group_root(buffer[i]),
                ntg_widget_size_map_get(_sizes, buffer[i]));
    }
}

void _ntg_widget_arrange_fn(
        const ntg_object* _widget,
        void* _layout_data,
        struct ntg_xy size,
        ntg_object_xy_map* out_positions,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    size_t i;
    size_t child_count = ntg_widget_get_children_count(widget);

    const ntg_widget** buffer = sarena_malloc(sizeof(void*) * child_count);
    assert(buffer != NULL);
    ntg_widget_get_children(widget, buffer, child_count);

    ntg_widget_xy_map* _positions;
    _positions = ntg_widget_xy_map_new(child_count, arena);
    for(i = 0; i < child_count; i++) ntg_widget_xy_map_set(_positions, buffer[i], 0);

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;
    layout_ops.arrange_fn(widget, _layout_data, orientation, size, _positions, arena);

    for(i = 0; i < child_count; i++)
    {
        ntg_object_xy_map_set(out_positions, ntg_widget_get_group_root(buffer[i]),
                ntg_widget_xy_map_get(_positions, buffer[i]));
    }
}

void _ntg_widget_draw_fn(
        const ntg_object* _widget,
        void* _layout_data,
        struct ntg_xy size,
        ntg_temp_object_drawing* out_drawing,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    size_t i, j;
    struct ntg_vcell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_temp_object_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = widget->_background;
        }
    }

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;
    if(layout_ops.draw_fn != NULL)
        layout_ops.draw_fn(widget, _layout_data, size, out_drawing, arena);

    assert(0);
}

static void init_default_values(ntg_widget* widget)
{
    _widget->__layout_ops = (struct ntg_widget_layout_ops) {0};
    _widget->__hooks = (struct ntg_widget_hooks) {0};
    widget->_background = ntg_vcell_default();
    widget->_user_min_size = ntg_xy(0, 0);
    widget->_user_max_size = ntg_xy(NTG_SIZE_MAX, NTG_SIZE_MAX);
    widget->_user_grow = ntg_xy(1, 1);
    widget->_border = NULL;
    widget->_padding = NULL;
    widget->__event_fn = NULL;
    widget->data = NULL;
}

static void object_rm_child_fn(ntg_object* _widget, ntg_object* child)
{
    ntg_widget* widget = (ntg_widget*)_widget;
    
    ntg_object* it = child;
    ntg_entity* _it = (ntg_entity*)it;
    while(!ntg_entity_instanceof(_it->_type, &NTG_ENTITY_WIDGET))
    {
        it = it->_children->data[0];
        _it = (ntg_entity*)it;
    }

    widget->__hooks.rm_child_fn(widget, it);
}
