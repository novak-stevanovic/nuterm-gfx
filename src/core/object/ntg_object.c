#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <stdlib.h>

/* ---------------------------------------------------------------- */
/* OBJECT TREE */
/* ---------------------------------------------------------------- */

const ntg_object* ntg_object_get_root(const ntg_object* object)
{
    assert(object != NULL);

    const ntg_object* it_obj = object;
    const ntg_object* prev = NULL;
    while(true)
    {
        prev = it_obj;
        it_obj = it_obj->_parent;
        
        if(it_obj == NULL) return prev;
    }

    return NULL;
}

ntg_object* ntg_object_get_root_(ntg_object* object)
{
    assert(object != NULL);

    return (ntg_object*)ntg_object_get_root(object);
}

bool ntg_object_is_ancestor(const ntg_object* object, const ntg_object* ancestor)
{
    assert(object != NULL);
    assert(ancestor != NULL);

    const ntg_object* it_object = object;
    while(it_object != NULL)
    {
        if(it_object == ancestor) return true;

        it_object = it_object->_parent;
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
/* LAYOUT */
/* ---------------------------------------------------------------- */

void ntg_object_root_layout(ntg_object* root, struct ntg_xy size)
{
    assert(root != NULL);
    assert(root->_parent == NULL);

    root->_pos = ntg_xy(0, 0);
    root->_size = size;
}

void ntg_object_measure(ntg_object* object, ntg_orient orient, bool constrained,
                        sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;

    struct ntg_object_measure measure = {0};
    if(object->__layout_ops.measure_fn != NULL)
    {
        measure = layout_ops.measure_fn(object, object->__layout_data,
                orient, constrained, arena);
    }

    if(orient == NTG_ORIENT_H)
    {
        object->_min_size.x = measure.min_size;
        object->_nat_size.x = measure.nat_size;
        object->_max_size.x = measure.max_size;
        object->_grow.x = measure.grow;
    }
    else // NTG_ORIENT_V
    {
        object->_min_size.y = measure.min_size;
        object->_nat_size.y = measure.nat_size;
        object->_max_size.y = measure.max_size;
        object->_grow.y = measure.grow;
    }
}

void ntg_object_constrain(ntg_object* object, ntg_orient orient, sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;
    ntg_object_vec* children = &object->_children;

    size_t size = ntg_object_get_size_1d(object, orient);

    size_t i;
    if((size == 0) || (layout_ops.constrain_fn == NULL))
    {
        for(i = 0; i < children->size; i++)
        {
            if(orient == NTG_ORIENT_H)
                children->data[i]->_size.x = 0;
            else
                children->data[i]->_size.y = 0;
        }
    }
    else
    {
        ntg_object_size_map* _sizes = ntg_object_size_map_new(children->size, arena);
        layout_ops.constrain_fn(object, object->__layout_data, orient, _sizes, arena);

        size_t it_size;
        for(i = 0; i < children->size; i++)
        {
            it_size = ntg_object_size_map_get(_sizes, children->data[i]);
            if(orient == NTG_ORIENT_H)
                children->data[i]->_size.x = it_size;
            else
                children->data[i]->_size.y = it_size;
        }
    }
}

void ntg_object_arrange(ntg_object* object, sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_xy size = object->_size;

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;
    ntg_object_vec* children = &object->_children;

    size_t i;
    if(ntg_xy_size_is_zero(size) || (layout_ops.arrange_fn == NULL))
    {
        for(i = 0; i < children->size; i++)
            children->data[i]->_pos = ntg_xy(0, 0);
    }
    else
    {
        ntg_object_xy_map* _poss = ntg_object_xy_map_new(children->size, arena);
        layout_ops.arrange_fn(object, object->__layout_data, _poss, arena);

        struct ntg_xy it_pos;
        for(i = 0; i < children->size; i++)
        {
            it_pos = ntg_object_xy_map_get(_poss, children->data[i]);

            children->data[i]->_pos = it_pos;
        }
    }
}

void ntg_object_draw(ntg_object* object, sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_xy size = object->_size;

    if(ntg_xy_size_is_zero(size)) return;

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;

    ntg_tmp_object_drawing _drawing;
    ntg_tmp_object_drawing_init(&_drawing, size, arena);

    if(layout_ops.draw_fn != NULL)
        layout_ops.draw_fn(object, object->__layout_data, &_drawing, arena);

    struct ntg_xy max_size = ntg_xy(NTG_SIZE_MAX, NTG_SIZE_MAX);
    if(object->_scene != NULL) max_size = object->_scene->_size;
    ntg_object_drawing_set_size(&object->_drawing, size, max_size);
    size_t i, j;
    struct ntg_vcell* it_cell;
    struct ntg_vcell* it_temp_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(&object->_drawing, ntg_xy(j, i));
            it_temp_cell = ntg_tmp_object_drawing_at_(&_drawing, ntg_xy(j, i));
            (*it_cell) = (*it_temp_cell);
        }
    }
}

struct ntg_xy ntg_object_get_pos_abs(const ntg_object* object)
{
    assert(object != NULL);

    const ntg_object* it_obj = object;
    struct ntg_xy pos = ntg_xy(0, 0);
    while(it_obj != NULL)
    {
        pos = ntg_xy_add(pos, it_obj->_pos);
        it_obj = it_obj->_parent;
    }

    return pos;
}

size_t ntg_object_get_for_size(const ntg_object* object,
        ntg_orient orient, bool constrained)
{
    assert(object != NULL);

    if(constrained)
    {
        if(orient == NTG_ORIENT_H)
            return object->_size.y;
        else
            return object->_size.x;
    }
    else
        return NTG_SIZE_MAX;
}

struct ntg_object_measure
ntg_object_get_measure(const ntg_object* object, ntg_orient orient)
{
    assert(object != NULL);

    return (struct ntg_object_measure) {
        .min_size = ntg_xy_get(object->_min_size, orient),
        .nat_size = ntg_xy_get(object->_nat_size, orient),
        .max_size = ntg_xy_get(object->_max_size, orient),
        .grow = ntg_xy_get(object->_grow, orient)
    };
}

size_t ntg_object_get_size_1d(const ntg_object* object, ntg_orient orient)
{
    assert(object != NULL);

    return ntg_xy_get(object->_size, orient);
}

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_object* object);

void ntg_object_init(ntg_object* object, struct ntg_object_layout_ops layout_ops)
{
    assert(object != NULL);

    init_default(object);

    ntg_object_vec_init(&object->_children, 2, NULL);
    object->_scene = NULL;
    object->__layout_ops = layout_ops;
    ntg_object_drawing_init(&object->_drawing);
}

static void init_default(ntg_object* object)
{
    object->_parent = NULL;
    object->_children = (ntg_object_vec) {0};
    object->_scene = NULL;

    object->__layout_ops = (struct ntg_object_layout_ops) {0};
    object->__layout_data = NULL;

    object->_min_size = ntg_xy(0, 0);
    object->_max_size = ntg_xy(0, 0);
    object->_nat_size = ntg_xy(0, 0);
    object->_grow = ntg_xy(1, 1);
    object->_size = ntg_xy(0, 0);
    object->_pos = ntg_xy(0, 0);
    object->__layout_data = NULL;
}

void ntg_object_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);
    
    ntg_object* object = (ntg_object*)entity;

    if(object->_parent)
        ntg_object_detach(object);

    while(object->_children.size > 0)
    {
        ntg_object_detach(object->_children.data[0]);
    }

    ntg_object_vec_deinit(&object->_children, NULL);
    ntg_object_drawing_deinit(&object->_drawing);

    init_default(object);
}

// enum ntg_object_addchld_status 
// ntg_object_validate_add_child(ntg_object* parent, ntg_object* child)
// {
//     assert(parent != NULL);
//     assert(child != NULL);
//     
//     if(ntg_object_vec_exists(&parent->_children, child, NULL))
//         return NTG_OBJECT_ADDCHLD_ALR_CHILD;
//     else if(child->_parent != NULL)
//         return NTG_OBJECT_ADDCHLD_HAS_PARENT;
//     else if(parent == child)
//         return NTG_OBJECT_ADDCHLD_SELF;
// 
//     return NTG_OBJECT_ADDCHLD_SUCCESS;
// }

void ntg_object_attach(ntg_object* parent, ntg_object* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);
    assert(child->_parent == NULL);

    if(child->_parent != NULL)
        ntg_object_detach(child);

    ntg_object_vec_pushb(&parent->_children, child, NULL);

    child->_parent = parent;

    struct ntg_event_object_chldadd_data data1 = { .child = child };
    ntg_entity_raise_event_((ntg_entity*)parent, NTG_EVENT_OBJECT_CHLDADD, &data1);

    struct ntg_event_object_prntchng_data data2 = {
        .old = NULL,
        .new = parent
    };
    ntg_entity_raise_event_((ntg_entity*)child, NTG_EVENT_OBJECT_PRNTCHNG, &data2);

    ntg_entity_raise_event_((ntg_entity*)parent, NTG_EVENT_OBJECT_DIFF, NULL);
}

void ntg_object_detach(ntg_object* widget)
{
    assert(widget != NULL);

    ntg_object* parent = widget->_parent;
    if(parent == NULL) return;

    ntg_object_vec_rm(&parent->_children, widget, NULL);

    widget->_parent = NULL;

    struct ntg_event_object_chldrm_data data1 = { .child = widget };
    ntg_entity_raise_event_((ntg_entity*)parent, NTG_EVENT_OBJECT_CHLDRM, &data1);

    struct ntg_event_object_prntchng_data data2 = {
        .old = parent,
        .new = NULL
    };
    ntg_entity_raise_event_((ntg_entity*)widget, NTG_EVENT_OBJECT_PRNTCHNG, &data2);

    ntg_entity_raise_event_((ntg_entity*)parent, NTG_EVENT_OBJECT_DIFF, NULL);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_object_set_scene(ntg_object* object, ntg_scene* scene)
{
    assert(object != NULL);

    ntg_scene* old = object->_scene;
    object->_scene = scene;

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;
    if(scene != NULL)
    {
        object->__layout_data = layout_ops.init_fn(object);
    }
    else
    {
        layout_ops.deinit_fn(object->__layout_data, object);
        object->__layout_data = NULL;
    }

    struct ntg_event_object_scnchng_data data = {
        .old = old,
        .new = scene
    };
    ntg_entity_raise_event_((ntg_entity*)object, NTG_EVENT_OBJECT_SCNCHNG, &data);
}
