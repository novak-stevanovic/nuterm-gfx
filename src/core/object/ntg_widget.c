#include <assert.h>
#include "ntg.h"
#include "core/entity/ntg_entity_type.h"
#include "shared/_ntg_shared.h"

static void init_default_values(ntg_widget* widget);

static void padding_remove(ntg_widget* widget);
static void padding_add(ntg_widget* widget, ntg_decorator* padding);
static void border_remove(ntg_widget* widget);
static void border_add(ntg_widget* widget, ntg_decorator* border);

/* -------------------------------------------------------------------------- */
/* CONTROL */
/* -------------------------------------------------------------------------- */

void ntg_widget_set_user_min_size(ntg_widget* widget, struct ntg_xy size)
{
    assert(widget != NULL);

    widget->_user_min_size = size;

    ntg_entity_raise_event((ntg_entity*)widget, NULL, NTG_EVENT_OBJECT_DIFF, NULL);
}

void ntg_widget_set_user_max_size(ntg_widget* widget, struct ntg_xy size)
{
    assert(widget != NULL);

    widget->_user_max_size = size;

    ntg_entity_raise_event((ntg_entity*)widget, NULL, NTG_EVENT_OBJECT_DIFF, NULL);
}

void ntg_widget_set_user_grow(ntg_widget* widget, struct ntg_xy grow)
{
    assert(widget != NULL);

    widget->_user_grow = grow;

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

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_min_size;
}

struct ntg_xy ntg_widget_get_cont_min_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_min_size;
}

struct ntg_xy ntg_widget_get_nat_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_nat_size;
}

struct ntg_xy ntg_widget_get_cont_nat_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_nat_size;
}

struct ntg_xy ntg_widget_get_max_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_max_size;
}

struct ntg_xy ntg_widget_get_cont_max_size(const ntg_widget* widget)
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

struct ntg_xy ntg_widget_get_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_size;
}

struct ntg_xy ntg_widget_get_cont_size(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_size;
}

struct ntg_xy ntg_widget_get_pos(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return group_root->_pos;
}

struct ntg_xy ntg_widget_get_pos_abs(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return ntg_object_get_pos_abs(group_root);
}

struct ntg_xy ntg_widget_get_cont_pos(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_pos;
}

struct ntg_xy ntg_widget_get_cont_pos_abs(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return ntg_object_get_pos_abs(_widget);
}

/* LAYOUT PROCESS - CONVENIENCE */

size_t ntg_widget_get_cont_for_size(const ntg_widget* widget,
        ntg_orient orient, bool constrain)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return ntg_object_get_for_size(_widget, orient, constrain);
}

struct ntg_object_measure
ntg_widget_get_measure(const ntg_widget* widget, ntg_orient orient)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return ntg_object_get_measure(group_root, orient);
}

struct ntg_object_measure
ntg_widget_get_cont_measure(const ntg_widget* widget, ntg_orient orient)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return ntg_object_get_measure(_widget, orient);
}

size_t ntg_widget_get_size_1d(const ntg_widget* widget, ntg_orient orient)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return ntg_xy_get(group_root->_size, orient);
}

size_t ntg_widget_get_cont_size_1d(const ntg_widget* widget, ntg_orient orient)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return ntg_xy_get(_widget->_size, orient);
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
            (const ntg_object*)descendant);
}

const ntg_widget* ntg_widget_get_parent(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* group_root = ntg_widget_get_group_root(widget);

    return (const ntg_widget*)group_root->_parent;
}

ntg_widget* ntg_widget_get_parent_(ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_object* group_root = ntg_widget_get_group_root_(widget);

    return (ntg_widget*)group_root->_parent;
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

    return _widget->_children.size;
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
    for(i = 0; ((i < cap) && (i < _widget->_children.size)); i++)
    {
        it = _widget->_children.data[i];
        _it = (const ntg_entity*)it;

        while(!ntg_entity_instance_of(_it->_type, &NTG_ENTITY_WIDGET))
        {
            it = it->_children.data[0];
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

    return ntg_widget_get_children(widget, (const ntg_widget**)out_children, cap);
}

void ntg_widget_set_padding(ntg_widget* widget, ntg_decorator* padding)
{
    assert(widget != NULL);

    if(widget->_padding == padding) return;

    if(padding && padding->_widget && padding->_widget->_padding)
        padding_remove(padding->_widget);

    if(widget->_padding != NULL)
        padding_remove(widget);

    if(padding)
        padding_add(widget, padding);
}

void ntg_widget_set_border(ntg_widget* widget, ntg_decorator* border)
{
    assert(widget != NULL);

    if(widget->_border == border) return;

    if(border && border->_widget && border->_widget->_border)
        border_remove(border->_widget);

    if(widget->_border != NULL)
        border_remove(widget);

    if(border)
        border_add(widget, border);
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
        .measure_fn = layout_ops.measure_fn ? _ntg_widget_measure_fn : NULL,
        .constrain_fn = layout_ops.constrain_fn ? _ntg_widget_constrain_fn : NULL,
        .arrange_fn = layout_ops.arrange_fn ? _ntg_widget_arrange_fn : NULL,
        .draw_fn = _ntg_widget_draw_fn,
        .init_fn = _ntg_widget_layout_data_init_fn,
        .deinit_fn = _ntg_widget_layout_data_deinit_fn,
    };

    ntg_object_init((ntg_object*)widget, _layout_ops);

    init_default_values(widget);

    widget->__layout_ops = layout_ops;
    widget->__hooks = hooks;
}

void ntg_widget_deinit_fn(ntg_entity* _widget)
{
    ntg_widget* widget = (ntg_widget*)_widget;

    init_default_values((ntg_widget*)_widget);

    if(widget->_padding != NULL)
        padding_remove(widget);
    if(widget->_border != NULL)
        border_remove(widget);

    ntg_object_deinit_fn(_widget);
}

void ntg_widget_attach(ntg_widget* parent, ntg_widget* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);
    assert(ntg_widget_get_parent(child) == NULL);

    ntg_object* _child = (ntg_object*)child;
    ntg_object* _parent = (ntg_object*)parent;

    assert(!(_child->_scene && (_child->_scene->_root == child)));

    ntg_object* group_root = ntg_widget_get_group_root_(child);

    ntg_object_attach(_parent, group_root);

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

void ntg_widget_detach(ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_widget* parent = ntg_widget_get_parent_(widget);

    if(parent == NULL) return;

    ntg_object* group_root = ntg_widget_get_group_root_(widget);

    ntg_object_detach(group_root);

    if(widget->__hooks.rm_child_fn != NULL)
        widget->__hooks.rm_child_fn(parent, widget);

    struct ntg_event_widget_chldrm_data data1 = { .child = widget };
    ntg_entity_raise_event((ntg_entity*)parent, NULL,
            NTG_EVENT_WIDGET_CHLDRM, &data1);

    struct ntg_event_widget_prntchng_data data2 = {
        .old = parent,
        .new = NULL
    };
    ntg_entity_raise_event((ntg_entity*)widget, NULL,
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
        ntg_orient orient,
        bool constrained,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;

    struct ntg_object_measure measure;
    measure = layout_ops.measure_fn(widget, _layout_data,
            orient, constrained, arena);

    size_t user_min_size = ntg_xy_get(widget->_user_min_size, orient);
    size_t user_max_size = ntg_xy_get(widget->_user_max_size, orient);
 
    user_min_size = _min2_size(user_min_size, user_max_size);
 
    if(user_max_size < measure.min_size)
        measure.min_size = user_max_size;
    else
        measure.min_size = _max2_size(measure.min_size, user_min_size);
 
    if(user_min_size > measure.max_size)
        measure.max_size = user_min_size;
    else
        measure.max_size = _min2_size(measure.max_size, user_max_size);
 
    measure.nat_size = _clamp_size(measure.min_size,
            measure.nat_size, measure.max_size);
 
    size_t user_grow = ntg_xy_get(widget->_user_grow, orient);
    measure.grow = (user_grow != NTG_WIDGET_GROW_UNSET) ?
            user_grow : measure.grow;

    return measure;
}

void _ntg_widget_constrain_fn(
        const ntg_object* _widget,
        void* _layout_data,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;
    struct ntg_xy size = _widget->_size;

    size_t i;
    size_t child_count = ntg_widget_get_children_count(widget);

    const ntg_widget** buffer = (const ntg_widget**)sarena_malloc(
            arena, sizeof(void*) * child_count);
    assert(buffer != NULL);
    ntg_widget_get_children(widget, buffer, child_count);

    ntg_widget_size_map* _sizes;
    _sizes = ntg_widget_size_map_new(child_count, arena);
    for(i = 0; i < child_count; i++) ntg_widget_size_map_set(_sizes, buffer[i], 0);

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;
    layout_ops.constrain_fn(widget, _layout_data, orient, _sizes, arena);

    for(i = 0; i < child_count; i++)
    {
        ntg_object_size_map_set(out_size_map, ntg_widget_get_group_root(buffer[i]),
                ntg_widget_size_map_get(_sizes, buffer[i]));
    }
}

void _ntg_widget_arrange_fn(
        const ntg_object* _widget,
        void* _layout_data,
        ntg_object_xy_map* out_pos_map,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;
    struct ntg_xy size = _widget->_size;

    size_t i;
    size_t child_count = ntg_widget_get_children_count(widget);

    const ntg_widget** buffer = sarena_malloc(arena, sizeof(void*) * child_count);
    assert(buffer != NULL);
    ntg_widget_get_children(widget, buffer, child_count);

    ntg_widget_xy_map* _poss;
    _poss = ntg_widget_xy_map_new(child_count, arena);
    for(i = 0; i < child_count; i++) ntg_widget_xy_map_set(_poss, buffer[i], ntg_xy(0, 0));

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;
    layout_ops.arrange_fn(widget, _layout_data, _poss, arena);

    for(i = 0; i < child_count; i++)
    {
        ntg_object_xy_map_set(out_pos_map, ntg_widget_get_group_root(buffer[i]),
                ntg_widget_xy_map_get(_poss, buffer[i]));
    }
}

void _ntg_widget_draw_fn(
        const ntg_object* _widget,
        void* _layout_data,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;
    struct ntg_xy size = _widget->_size;

    size_t i, j;
    struct ntg_vcell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = widget->_background;
        }
    }

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;
    if(layout_ops.draw_fn != NULL)
        layout_ops.draw_fn(widget, _layout_data, out_drawing, arena);
}

static void init_default_values(ntg_widget* widget)
{
    widget->__layout_ops = (struct ntg_widget_layout_ops) {0};
    widget->__hooks = (struct ntg_widget_hooks) {0};
    widget->_background = ntg_vcell_default();
    widget->_user_min_size = ntg_xy(NTG_WIDGET_MIN_SIZE_UNSET, NTG_WIDGET_MIN_SIZE_UNSET);
    widget->_user_max_size = ntg_xy(NTG_WIDGET_MAX_SIZE_UNSET, NTG_WIDGET_MAX_SIZE_UNSET);
    widget->_user_grow = ntg_xy(NTG_WIDGET_GROW_UNSET, NTG_WIDGET_GROW_UNSET);
    widget->_border = NULL;
    widget->_padding = NULL;
    widget->__event_fn = NULL;
    widget->data = NULL;
}

static void padding_remove(ntg_widget* widget)
{
    assert(widget->_padding != NULL);

    ntg_object* _widget = (ntg_object*)widget;

    ntg_decorator* padding = widget->_padding;
    ntg_object* _padding = (ntg_object*)padding;
    ntg_object* padding_parent = (widget->_border != NULL) ?
            (ntg_object*)widget->_border :
            (ntg_object*)ntg_widget_get_parent_(widget);

    ntg_object_detach(_widget);

    if(padding_parent != NULL)
    {
        ntg_object_detach(_padding);
        ntg_object_attach(padding_parent, _widget);
    }

    _ntg_decorator_decorate(padding, NULL);
    widget->_padding = NULL;

    struct ntg_event_widget_padchng_data data = {
        .old = padding,
        .new = NULL
    };

    ntg_entity_raise_event((ntg_entity*)widget, NULL,
                           NTG_EVENT_WIDGET_PADCHNG, &data);
}

static void padding_add(ntg_widget* widget, ntg_decorator* padding)
{
    assert(widget->_padding == NULL);

    ntg_object* _widget = (ntg_object*)widget;

    ntg_object* _padding = (ntg_object*)padding;
    ntg_object* padding_parent = (widget->_border != NULL) ?
            (ntg_object*)widget->_border :
            (ntg_object*)ntg_widget_get_parent_(widget);

    ntg_object_detach(_widget);
    if(padding_parent != NULL)
        ntg_object_attach(padding_parent, _padding);
    ntg_object_attach(_padding, _widget);

    _ntg_decorator_decorate(padding, widget);
    widget->_padding = padding;

    struct ntg_event_widget_padchng_data data = {
        .old = NULL,
        .new = padding
    };

    ntg_entity_raise_event((ntg_entity*)widget, NULL,
                           NTG_EVENT_WIDGET_PADCHNG, &data);
}

static void border_remove(ntg_widget* widget)
{
    assert(widget->_border != NULL);

    ntg_object* _widget = (ntg_object*)widget;

    ntg_decorator* border = widget->_border;
    ntg_object* _border = (ntg_object*)border;
    ntg_object* border_parent = (ntg_object*)ntg_widget_get_parent_(widget);
    ntg_object* border_child = (widget->_padding != NULL) ?
            (ntg_object*)widget->_padding : _widget;

    ntg_object_detach(border_child);

    if(border_parent != NULL)
    {
        ntg_object_detach(_border);
        ntg_object_attach(border_parent, border_child);
    }

    _ntg_decorator_decorate(border, NULL);
    widget->_border = NULL;

    struct ntg_event_widget_bordchng_data data = {
        .old = border,
        .new = NULL
    };

    ntg_entity_raise_event((ntg_entity*)widget, NULL,
                           NTG_EVENT_WIDGET_BORDCHNG, &data);
}

static void border_add(ntg_widget* widget, ntg_decorator* border)
{
    assert(widget->_border == NULL);

    ntg_object* _widget = (ntg_object*)widget;

    ntg_object* _border = (ntg_object*)border;
    ntg_object* border_parent = (ntg_object*)ntg_widget_get_parent_(widget);
    ntg_object* border_child = (widget->_padding != NULL) ?
            (ntg_object*)widget->_padding : _widget;

    if(border_parent != NULL)
    {
        ntg_object_detach(border_child);
        ntg_object_attach(border_parent, _border);
    }
    ntg_object_attach(_border, border_child);

    _ntg_decorator_decorate(border, widget);
    widget->_border = border;

    struct ntg_event_widget_bordchng_data data = {
        .old = NULL,
        .new = border
    };

    ntg_entity_raise_event((ntg_entity*)widget, NULL,
                           NTG_EVENT_WIDGET_BORDCHNG, &data);
}
