#include "ntg.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <stdlib.h>

static void set_scene(ntg_object* object, ntg_scene* scene);

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

    root->_position = ntg_xy(0, 0);
    root->_size = size;
}

void ntg_object_measure(
        ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;

    struct ntg_object_measure measure = {0};
    if(object->__layout_ops.measure_fn != NULL)
    {
        measure = layout_ops.measure_fn(object,
            object->__layout_data, orientation, arena);
    }

    if(orientation == NTG_ORIENT_H)
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

void ntg_object_constrain(
        ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;
    ntg_object_vec* children = &object->_children;

    if((size == 0) || (layout_ops.constrain_fn == NULL))
    {
        size_t i;
        for(i = 0; i < children->size; i++)
        {
            if(orientation == NTG_ORIENT_H)
                children->data[i]._size.x = 0;
            else
                children->data[i]._size.y = 0;
        }
    }
    else
    {
        ntg_object_size_map* _sizes = ntg_object_size_map_new(children->size, arena);
        layout_ops.constrain_fn(object, object->__layout_data,
                orientation, size, _sizes, arena);
    }
}

void ntg_object_arrange(
        ntg_object* object,
        struct ntg_xy size,
        sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;
    ntg_object_vec* children = &object->_children;

    if(ntg_xy_size_is_zero(size) || (layout_ops.arrange_fn == NULL))
    {
        size_t i;
        for(i = 0; i < children->size; i++)
            children->data[i]._position = ntg_xy(0, 0);
    }
    else
    {
        ntg_object_xy_map* _positions = ntg_object_xy_map_new(children->size, arena);
        layout_ops.arrange_fn(object, object->__layout_data, size, _positions, arena);
    }
}

void ntg_object_draw(
        const ntg_object* object,
        sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_xy size = object->_size;

    if(ntg_xy_size_is_zero(size)) return;

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;

    ntg_temp_object_drawing _drawing;
    ntg_temp_object_drawing_init(&_drawing, size, arena);

    if(layout_ops.draw_fn != NULL)
        layout_ops.draw_fn(object, object->__layout_data, size, &_drawing, arena);

    ntg_object_drawing_set_size(&object->_drawing, size);
    size_t i, j;
    struct ntg_vcell* it_cell;
    struct ntg_vcell* it_temp_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(&object->_drawing, ntg_xy(j, i));
            it_temp_cell = ntg_temp_object_drawing_at_(&_drawing, ntg_xy(j, i));
            (*it_cell) = (*it_temp_cell);
        }
    }
}

struct ntg_xy ntg_object_get_position_abs(const ntg_object* object)
{
    assert(object != NULL);

    const ntg_object* s = (mode == NTG_OBJECT_GROUP_ROOT) ?
        ntg_object_get_group_root(object) :
        object;

    const ntg_object* it_obj = s;
    struct ntg_xy pos = ntg_xy(0, 0);
    while(it_obj != NULL)
    {
        pos = ntg_xy_add(pos, ntg_object_get_position(it_obj, NTG_OBJECT_SELF));
        it_obj = ntg_object_get_parent(it_obj, true);
    }

    return pos;
}

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

static void init_default_values(ntg_object* object);

void ntg_object_init(ntg_object* object,
        struct ntg_object_layout_ops layout_ops,
        struct ntg_object_hooks hooks)
{
    assert(object != NULL);

    init_default_values(object);

    ntg_object_vec_init(&object->_children, 2, NULL);
    object->_scene = NULL;
    object->__layout_ops = layout_ops;
    object->__hooks = hooks;
    ntg_object_drawing_init(&object->__drawing);
}

static void init_default_values(ntg_object* object)
{
    object->_parent = NULL;
    object->_children = (ntg_object_vec) {0};
    object->_scene = NULL;

    object->__layout_ops = (struct ntg_object_layout_ops) {0};
    object->__hooks = (struct ntg_object_hooks) {0};
    object->__layout_data = NULL;

    object->_min_size = ntg_xy(0, 0);
    object->_max_size = ntg_xy(0, 0);
    object->_nat_size = ntg_xy(0, 0);
    object->_size = ntg_xy(0, 0);
    object->_position = ntg_xy(0, 0);
    object->__layout_data = NULL;
}

void ntg_object_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_object* object = (ntg_object*)entity;
    ntg_object_vec_deinit(&object->_children, NULL);
    ntg_object_drawing_deinit(&object->_drawing);

    init_default_values(object);
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

void ntg_object_add_child(ntg_object* parent, ntg_object* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);
    if(child->_parent == parent) return;

    if(child->_parent != NULL)
        ntg_object_rm_child(child->_parent, child);

    ntg_object_vec_pushb(parent, child, NULL);

    child->_parent = parent;

    struct ntg_event_object_chldadd_data data1 = { .child = child };
    ntg_entity_raise_event((ntg_entity*)parent, NULL,
            NTG_EVENT_OBJECT_CHLDADD, &data1);

    struct ntg_event_object_prntchng data2 = {
        .old = NULL,
        .new = parent
    };
    ntg_entity_raise_event((ntg_entity*)child, NULL,
            NTG_EVENT_OBJECT_PRNTCHNG, &data2);

    if(parent->_scene != NULL)
        set_scene(child, parent->_scene);
}

void ntg_object_rm_child(ntg_object* parent, ntg_object* child)
{
    assert(parent != NULL);
    assert(child != NULL);
    assert(parent != child);
    assert(child->_parent == parent);

    ntg_object_vec_rm(parent, child, NULL);

    child->_parent = NULL;

    if(parent->__hooks.rm_child_fn != NULL)
    {
        parent->__hooks.rm_child_fn(parent, child);
    }

    struct ntg_event_object_chldrm_data data1 = { .child = child };
    ntg_entity_raise_event((ntg_entity*)parent, NULL,
            NTG_EVENT_OBJECT_CHLDRM, &data1);

    struct ntg_event_object_parent_change data2 = {
        .old = parent,
        .new = NULL
    };
    ntg_entity_raise_event((ntg_entity*)child, NULL,
            NTG_EVENT_OBJECT_PRNTCHNG, &data2);

    if(child->_scene != NULL)
        set_scene(child, NULL);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

static void set_scene(ntg_object* object, ntg_scene* scene)
{
    ntg_scene* scene = (ntg_scene*)_scene;
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

    struct ntg_event_object_scnchng data = {
        .old = object->_scene,
        .new = scene
    };
    ntg_entity_raise_event((ntg_entity*)object, NULL,
            NTG_EVENT_OBJECT_SCNCHNG, &data);
}

static void set_scene_fn(ntg_object* object, void* _scene)
{
    set_scene(object, (ntg_scene*)_scene);
}

NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(set_scene_tree, set_scene_fn);

void _ntg_object_root_set_scene(ntg_object* root, ntg_scene* scene)
{
    assert(root != NULL);
    assert(root->_parent == NULL);

    set_scene_tree(root, scene);
}
