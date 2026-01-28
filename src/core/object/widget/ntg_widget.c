#include <assert.h>
#include <stdlib.h>
#include "ntg.h"
#include "shared/_ntg_shared.h"

static void init_default(ntg_widget* widget);

static void padding_remove(ntg_widget* widget);
static void padding_add(ntg_widget* widget, ntg_decorator* padding);
static void border_remove(ntg_widget* widget);
static void border_add(ntg_widget* widget, ntg_decorator* border);

/* -------------------------------------------------------------------------- */
/* CONTROL */
/* -------------------------------------------------------------------------- */

// uninited vec
void ntg_widget_set_cont_min_size(ntg_widget* widget, struct ntg_xy size)
{
    assert(widget != NULL);

    widget->_cont_min_size = size;

    ntg_object_add_dirty((ntg_object*)widget, NTG_OBJECT_DIRTY_FULL);
}

void ntg_widget_set_cont_max_size(ntg_widget* widget, struct ntg_xy size)
{
    assert(widget != NULL);

    widget->_cont_max_size = size;

    ntg_object_add_dirty((ntg_object*)widget, NTG_OBJECT_DIRTY_FULL);
}

void ntg_widget_set_grow(ntg_widget* widget, struct ntg_xy grow)
{
    assert(widget != NULL);

    widget->_grow = grow;

    ntg_object_add_dirty((ntg_object*)widget, NTG_OBJECT_DIRTY_FULL);
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
    ntg_entity_raise_event_((ntg_entity*)widget, NTG_EVENT_WIDGET_BGCHNG, &data);
    ntg_object_add_dirty((ntg_object*)widget, NTG_OBJECT_DIRTY_DRAW);
}

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

void ntg_widget_set_on_key_fn(ntg_widget* widget,
        bool (*on_key_fn)(ntg_widget* widget, struct nt_key_event key))
{
    assert(widget);

    widget->__on_key_fn = on_key_fn;
}

void ntg_widget_set_on_mouse_fn(ntg_widget* widget,
        bool (*on_mouse_fn)(ntg_widget* widget, struct nt_mouse_event mouse))
{
    assert(widget);

    widget->__on_mouse_fn = on_mouse_fn;
}

bool ntg_widget_on_key(ntg_widget* widget, struct nt_key_event key)
{
    assert(widget);
    
    if(widget->__on_key_fn)
        return widget->__on_key_fn(widget, key);
    else
        return false;
}

bool ntg_widget_on_mouse(ntg_widget* widget, struct nt_mouse_event mouse)
{
    assert(widget);

    if(widget->__on_mouse_fn)
        return widget->__on_mouse_fn(widget, mouse);
    else
        return false;
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

int ntg_widget_get_z_index(const ntg_widget* widget)
{
    assert(widget != NULL);

    const ntg_object* _widget = (const ntg_object*)widget;

    return _widget->_z_index;
}

void ntg_widget_set_z_index(ntg_widget* widget, int z_index)
{
    assert(widget != NULL);

    ntg_object_set_z_index((ntg_object*)widget, z_index);
    if(widget->_padding)
        ntg_object_set_z_index((ntg_object*)widget->_padding, z_index);
    if(widget->_border)
        ntg_object_set_z_index((ntg_object*)widget->_border, z_index);
}

/* LAYOUT PROCESS - CONVENIENCE */

size_t ntg_widget_get_cont_for_size(
        const ntg_widget* widget,
        ntg_orient orient,
        bool constrain)
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

bool ntg_widget_is_ancestor_eq(const ntg_widget* widget, const ntg_widget* ancestor)
{
    return ((widget == ancestor) || ntg_widget_is_ancestor(widget, ancestor));
}

bool ntg_widget_is_descendant(const ntg_widget* widget, const ntg_widget* descendant)
{
    assert(widget != NULL);
    assert(descendant != NULL);

    return ntg_object_is_descendant((const ntg_object*)widget,
            (const ntg_object*)descendant);
}

bool ntg_widget_is_descendant_eq(const ntg_widget* widget, const ntg_widget* descendant)
{
    return ((widget == descendant) || ntg_widget_is_descendant(widget, descendant));
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

size_t ntg_widget_get_children(const ntg_widget* widget, ntg_widget** out_buff)
{
    assert(widget != NULL);

    ntg_object* _widget = (ntg_object*)widget;
    ntg_object_vec* children = &_widget->_children;
    if(children->size == 0) return 0;

    if(out_buff)
    {
        ntg_object* it;
        size_t i;
        for(i = 0; i < children->size; i++)
        {
            it = children->data[i];
        
            if(it->_type == NTG_OBJECT_DECORATOR)
                it = (ntg_object*)(((ntg_decorator*)it)->_widget);

            out_buff[i] = (ntg_widget*)it;
        }
    }

    return children->size;
}

size_t ntg_widget_get_children_by_z(const ntg_widget* widget, ntg_widget** out_buff)
{
    assert(widget);

    ntg_object* _widget = (ntg_object*)widget;
    ntg_object_vec* children = &_widget->_children;
    if(children->size == 0) return 0;

    if(out_buff)
    {
        ntg_widget_get_children(widget, out_buff);
        ntg_widget* tmp_obj;
        size_t i, j;
        for(i = 0; i < children->size - 1; i++)
        {
            for(j = i + 1; j < children->size; j++)
            {
                if(((ntg_object*)out_buff[j])->_z_index < 
                ((ntg_object*)out_buff[i])->_z_index)
                {
                    tmp_obj = out_buff[i];
                    out_buff[i] = out_buff[j];
                    out_buff[j] = tmp_obj;
                }
            }
        }
    }

    return children->size;
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

struct ntg_xy ntg_widget_map_to_ancestor_space(
        const ntg_widget* widget_space,
        const ntg_widget* ancestor_space,
        struct ntg_xy point)
{
    assert(widget_space != NULL);

    if(widget_space == ancestor_space)
        return point;

    const ntg_object* wroot = ntg_widget_get_group_root(widget_space);
    const ntg_object* aroot = (ancestor_space != NULL) ?
            ntg_widget_get_group_root(ancestor_space) :
            NULL;

    return ntg_object_map_to_ancestor_space(wroot, aroot, point);
}

struct ntg_xy ntg_widget_map_to_descendant_space(
        const ntg_widget* widget_space,
        const ntg_widget* descendant_space,
        struct ntg_xy point)
{
    assert(descendant_space != NULL);

    if(widget_space == descendant_space)
        return point;

    const ntg_object* aroot = (widget_space != NULL)
            ? ntg_widget_get_group_root(widget_space)
            : NULL;
    const ntg_object* droot = ntg_widget_get_group_root(descendant_space);

    return ntg_object_map_to_descendant_space(aroot, droot, point);
}

struct ntg_xy ntg_widget_map_to_scene_space(
        const ntg_widget* widget_space,
        struct ntg_xy point)
{
    assert(widget_space != NULL);

    const ntg_object* wroot = ntg_widget_get_group_root(widget_space);
    return ntg_object_map_to_ancestor_space(wroot, NULL, point);
}

struct ntg_xy ntg_widget_map_from_scene_space(
        const ntg_widget* widget_space,
        struct ntg_xy point)
{
    assert(widget_space != NULL);

    const ntg_object* wroot = ntg_widget_get_group_root(widget_space);
    return ntg_object_map_to_descendant_space(NULL, wroot, point);
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
        .init_fn = _ntg_widget_layout_init_fn,
        .deinit_fn = _ntg_widget_layout_deinit_fn,
    };

    ntg_object_init((ntg_object*)widget, _layout_ops, NTG_OBJECT_BASE);

    init_default(widget);

    widget->__layout_ops = layout_ops;
    widget->__hooks = hooks;
}

void ntg_widget_deinit(ntg_widget* widget)
{
    ntg_object* object = (ntg_object*)widget;

    if(object->_scene && (object->_scene->_root == widget))
        ntg_scene_set_root(object->_scene, NULL);

    if(widget->_padding != NULL)
        padding_remove(widget);
    if(widget->_border != NULL)
        border_remove(widget);

    ntg_widget* parent = ntg_widget_get_parent_(widget);
    if(parent != NULL) ntg_widget_detach(widget);

    size_t children_size = ntg_widget_get_children(widget, NULL);
    ntg_widget** children = malloc(sizeof(ntg_widget*) * children_size);
    assert(children);
    ntg_widget_get_children(widget, children);

    size_t i;
    for(i = 0; i < children_size; i++)
        ntg_widget_detach(children[i]);

    init_default(widget);

    ntg_object_deinit((ntg_object*)widget);

    free(children);
}

void ntg_widget_attach(ntg_widget* parent, ntg_widget* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);

    ntg_widget* curr_parent = ntg_widget_get_parent_(child);
    if(curr_parent != NULL)
    {
        ntg_widget_detach(child);
    }

    ntg_object* _child = (ntg_object*)child;
    ntg_object* _parent = (ntg_object*)parent;

    assert(!(_child->_scene && (_child->_scene->_root == child)));

    ntg_object* group_root = ntg_widget_get_group_root_(child);

    ntg_object_attach(_parent, group_root);

    struct ntg_event_widget_chldadd_data data1 = { .child = child };
    ntg_entity_raise_event_((ntg_entity*)parent, NTG_EVENT_WIDGET_CHLDADD, &data1);

    struct ntg_event_widget_prntchng_data data2 = {
        .old = NULL,
        .new = parent
    };
    ntg_entity_raise_event_((ntg_entity*)child, NTG_EVENT_WIDGET_PRNTCHNG, &data2);
}

void ntg_widget_detach(ntg_widget* widget)
{
    assert(widget != NULL);

    ntg_widget* parent = ntg_widget_get_parent_(widget);

    if(parent == NULL) return;

    ntg_object* group_root = ntg_widget_get_group_root_(widget);

    ntg_object_detach(group_root);

    if(parent->__hooks.on_rm_child_fn != NULL)
        parent->__hooks.on_rm_child_fn(parent, widget);

    struct ntg_event_widget_chldrm_data data1 = { .child = widget };
    ntg_entity_raise_event_((ntg_entity*)parent, NTG_EVENT_WIDGET_CHLDRM, &data1);

    struct ntg_event_widget_prntchng_data data2 = {
        .old = parent,
        .new = NULL
    };
    ntg_entity_raise_event_((ntg_entity*)widget, NTG_EVENT_WIDGET_PRNTCHNG, &data2);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void* _ntg_widget_layout_init_fn(const ntg_object* _widget)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    if(widget->__layout_ops.init_fn != NULL)
        return widget->__layout_ops.init_fn(widget);
    else
        return NULL;
}

void _ntg_widget_layout_deinit_fn(void* data, const ntg_object* _widget)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    if(widget->__layout_ops.deinit_fn != NULL)
        widget->__layout_ops.deinit_fn(data, widget);
}

struct ntg_object_measure _ntg_widget_measure_fn(
        const ntg_object* _widget,
        void* _ldata,
        ntg_orient orient,
        bool constrained,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;

    struct ntg_object_measure measure;
    measure = layout_ops.measure_fn(widget, _ldata,
            orient, constrained, arena);

    size_t cont_min_size = ntg_xy_get(widget->_cont_min_size, orient);
    size_t cont_max_size = ntg_xy_get(widget->_cont_max_size, orient);
 
    cont_min_size = _min2_size(cont_min_size, cont_max_size);
 
    if(cont_max_size < measure.min_size)
        measure.min_size = cont_max_size;
    else
        measure.min_size = _max2_size(measure.min_size, cont_min_size);
 
    if(cont_min_size > measure.max_size)
        measure.max_size = cont_min_size;
    else
        measure.max_size = _min2_size(measure.max_size, cont_max_size);
 
    measure.nat_size = _clamp_size(measure.min_size,
            measure.nat_size, measure.max_size);
 
    size_t user_grow = ntg_xy_get(widget->_grow, orient);
    measure.grow = (user_grow != NTG_WIDGET_GROW_UNSET) ?
            user_grow : measure.grow;

    return measure;
}

void _ntg_widget_constrain_fn(
        const ntg_object* _widget,
        void* _ldata,
        ntg_orient orient,
        ntg_object_size_map* out_size_map,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;
    struct ntg_xy size = _widget->_size;

    size_t children_size = ntg_widget_get_children(widget, NULL);
    ntg_widget** children = sarena_malloc(arena, sizeof(void*) * children_size);
    assert(children);
    ntg_widget_get_children(widget, children);

    ntg_widget_size_map* _sizes;
    _sizes = ntg_widget_size_map_new(children_size, arena);
    size_t i;
    for(i = 0; i < children_size; i++)
            ntg_widget_size_map_set(_sizes, children[i], 0);

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;
    layout_ops.constrain_fn(widget, _ldata, orient, _sizes, arena);

    for(i = 0; i < children_size; i++)
    {
        ntg_object_size_map_set(out_size_map,
                ntg_widget_get_group_root(children[i]),
                ntg_widget_size_map_get(_sizes, children[i]));
    }
}

void _ntg_widget_arrange_fn(
        const ntg_object* _widget,
        void* _ldata,
        ntg_object_xy_map* out_pos_map,
        sarena* arena)
{
    const ntg_widget* widget = (const ntg_widget*)_widget;
    struct ntg_xy size = _widget->_size;

    size_t children_size = ntg_widget_get_children(widget, NULL);
    ntg_widget** children = sarena_malloc(arena, sizeof(void*) * children_size);
    assert(children);
    ntg_widget_get_children(widget, children);

    ntg_widget_xy_map* _poss;
    _poss = ntg_widget_xy_map_new(children_size, arena);
    size_t i;
    for(i = 0; i < children_size; i++)
            ntg_widget_xy_map_set(_poss, children[i], ntg_xy(0, 0));

    struct ntg_widget_layout_ops layout_ops = widget->__layout_ops;
    layout_ops.arrange_fn(widget, _ldata, _poss, arena);

    for(i = 0; i < children_size; i++)
    {
        ntg_object_xy_map_set(out_pos_map,
                ntg_widget_get_group_root(children[i]),
                ntg_widget_xy_map_get(_poss, children[i]));
    }
}

void _ntg_widget_draw_fn(
        const ntg_object* _widget,
        void* _ldata,
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
        layout_ops.draw_fn(widget, _ldata, out_drawing, arena);
}

static void init_default(ntg_widget* widget)
{
    widget->__layout_ops = (struct ntg_widget_layout_ops) {0};
    widget->__hooks = (struct ntg_widget_hooks) {0};
    widget->_background = ntg_vcell_default();
    widget->_cont_min_size = ntg_xy(NTG_WIDGET_MIN_SIZE_UNSET, NTG_WIDGET_MIN_SIZE_UNSET);
    widget->_cont_max_size = ntg_xy(NTG_WIDGET_MAX_SIZE_UNSET, NTG_WIDGET_MAX_SIZE_UNSET);
    widget->_grow = ntg_xy(NTG_WIDGET_GROW_UNSET, NTG_WIDGET_GROW_UNSET);
    widget->_border = NULL;
    widget->_padding = NULL;

    widget->__on_key_fn = NULL;
    widget->__on_mouse_fn = NULL;
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

    ntg_scene* scene = _widget->_scene;
    if(scene && !ntg_widget_get_parent(widget)) // ROOT
    {
        ntg_scene_set_root(scene, NULL);
    }

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

    if(scene && !ntg_widget_get_parent(widget)) // ROOT
    {
        ntg_scene_set_root(scene, widget);
    }

    ntg_entity_raise_event_((ntg_entity*)widget, NTG_EVENT_WIDGET_PADCHNG, &data);
}

static void padding_add(ntg_widget* widget, ntg_decorator* padding)
{
    assert(widget->_padding == NULL);

    ntg_object* _widget = (ntg_object*)widget;

    ntg_object* _padding = (ntg_object*)padding;
    ntg_object* padding_parent = (widget->_border != NULL) ?
            (ntg_object*)widget->_border :
            (ntg_object*)ntg_widget_get_parent_(widget);

    ntg_scene* scene = _widget->_scene;
    if(scene && !ntg_widget_get_parent(widget)) // ROOT
    {
        ntg_scene_set_root(scene, NULL);
    }

    ntg_object_detach(_widget);
    if(padding_parent != NULL)
        ntg_object_attach(padding_parent, _padding);
    ntg_object_attach(_padding, _widget);

    _ntg_decorator_decorate(padding, widget);
    widget->_padding = padding;

    if(scene && !ntg_widget_get_parent(widget)) // ROOT
    {
        ntg_scene_set_root(scene, widget);
    }

    struct ntg_event_widget_padchng_data data = {
        .old = NULL,
        .new = padding
    };

    ntg_entity_raise_event_((ntg_entity*)widget, NTG_EVENT_WIDGET_PADCHNG, &data);
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

    ntg_scene* scene = _widget->_scene;
    if(scene && !ntg_widget_get_parent(widget)) // ROOT
    {
        ntg_scene_set_root(scene, NULL);
    }

    ntg_object_detach(border_child);

    if(border_parent != NULL)
    {
        ntg_object_detach(_border);
        ntg_object_attach(border_parent, border_child);
    }

    _ntg_decorator_decorate(border, NULL);
    widget->_border = NULL;

    if(scene && !ntg_widget_get_parent(widget)) // ROOT
    {
        ntg_scene_set_root(scene, widget);
    }

    struct ntg_event_widget_bordchng_data data = {
        .old = border,
        .new = NULL
    };

    ntg_entity_raise_event_((ntg_entity*)widget, NTG_EVENT_WIDGET_BORDCHNG, &data);
}

static void border_add(ntg_widget* widget, ntg_decorator* border)
{
    assert(widget->_border == NULL);

    ntg_object* _widget = (ntg_object*)widget;

    ntg_object* _border = (ntg_object*)border;
    ntg_object* border_parent = (ntg_object*)ntg_widget_get_parent_(widget);
    ntg_object* border_child = (widget->_padding != NULL) ?
            (ntg_object*)widget->_padding : _widget;

    ntg_scene* scene = _widget->_scene;
    if(scene && !ntg_widget_get_parent(widget)) // ROOT
    {
        ntg_scene_set_root(scene, NULL);
    }

    if(border_parent != NULL)
    {
        ntg_object_detach(border_child);
        ntg_object_attach(border_parent, _border);
    }
    ntg_object_attach(_border, border_child);

    _ntg_decorator_decorate(border, widget);
    widget->_border = border;

    if(scene && !ntg_widget_get_parent(widget)) // ROOT
    {
        ntg_scene_set_root(scene, widget);
    }

    struct ntg_event_widget_bordchng_data data = {
        .old = NULL,
        .new = border
    };

    ntg_entity_raise_event_((ntg_entity*)widget, NTG_EVENT_WIDGET_BORDCHNG, &data);
}
