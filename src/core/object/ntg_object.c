#include <stdlib.h>
#include <assert.h>
#include "ntg.h"
#include "shared/_ntg_shared.h"

static void object_register_fn(ntg_object* object, void* _scene)
{
    if(_scene)
        _ntg_scene_object_register((ntg_scene*)_scene, object);
}

static void object_unregister_fn(ntg_object* object, void* _)
{
    if(object->_scene)
        _ntg_scene_object_unregister(object->_scene, object);
}

NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(object_register_tree, object_register_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(object_unregister_tree, object_unregister_fn);

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

bool ntg_object_is_ancestor_eq(const ntg_object* object, const ntg_object* ancestor)
{
    return ((object == ancestor) || ntg_object_is_ancestor(object, ancestor));
}

bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant)
{
    assert(object != NULL);
    assert(descendant != NULL);

    return ntg_object_is_ancestor(descendant, object);
}

bool ntg_object_is_descendant_eq(const ntg_object* object, const ntg_object* descendant)
{
    return ((object == descendant) || ntg_object_is_descendant(object, descendant));
}

static void count_fn(ntg_object* object, void* _counter)
{
    if(!object) return;

    size_t* counter = _counter;
    (*counter)++;
}

NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(count_tree, count_fn);

size_t ntg_object_get_tree_size(const ntg_object* root)
{
    assert(root);

    size_t counter = 0;
    count_tree((ntg_object*)root, &counter);

    return counter;
}

size_t ntg_object_get_children_by_z(const ntg_object* object, ntg_object** out_buff)
{
    assert(object);

    ntg_object_vec* children = &object->_children;
    if(children->size == 0) return 0;

    if(out_buff)
    {
        size_t i, j;

        for(i = 0; i < children->size; i++)
            out_buff[i] = children->data[i];

        ntg_object* tmp_obj;
        for(i = 0; i < children->size - 1; i++)
        {
            for(j = i + 1; j < children->size; j++)
            {
                if((out_buff[j])->_z_index < (out_buff[i])->_z_index)
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

struct ntg_xy ntg_object_map_to_ancestor_space(
        const ntg_object* object_space,
        const ntg_object* ancestor_space,
        struct ntg_xy point)
{
    assert(object_space != NULL);

    if(object_space == ancestor_space)
        return point;

    struct ntg_xy out = point;

    const ntg_object* it = object_space;
    while(it != NULL && it != ancestor_space)
    {
        out = ntg_xy_add(out, it->_pos);
        it = it->_parent;
    }

    /* If ancestor_space is non-NULL, it must be reached (must be an ancestor). */
    if(ancestor_space != NULL)
        assert(it == ancestor_space);

    return out;
}

struct ntg_xy ntg_object_map_to_descendant_space(
        const ntg_object* object_space,
        const ntg_object* descendant_space,
        struct ntg_xy point)
{
    assert(descendant_space != NULL);

    if(object_space == descendant_space)
        return point;

    struct ntg_xy desc_pos = ntg_object_map_to_ancestor_space(
            descendant_space, object_space, ntg_xy(0, 0));

    return ntg_xy_sub(point, desc_pos);
}

struct ntg_xy ntg_object_map_to_scene_space(
        const ntg_object* object_space,
        struct ntg_xy point)
{
    assert(object_space != NULL);

    return ntg_object_map_to_ancestor_space(object_space, NULL, point);
}

struct ntg_xy ntg_object_map_from_scene_space(
        const ntg_object* object_space,
        struct ntg_xy point)
{
    assert(object_space != NULL);

    return ntg_object_map_to_descendant_space(NULL, object_space, point);
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

void ntg_object_measure(
        ntg_object* object,
        ntg_orient orient,
        bool constrained,
        sarena* arena)
{
    assert(object != NULL);
    assert(arena != NULL);

    struct ntg_object_layout_ops layout_ops = object->__layout_ops;

    struct ntg_object_measure old = ntg_object_get_measure(object, orient);

    struct ntg_object_measure measure = {0};
    if(object->__layout_ops.measure_fn != NULL)
    {
        measure = layout_ops.measure_fn(object, object->__ldata,
                orient, constrained, arena);
    }

    if(object->_parent && !ntg_object_measure_are_equal(old, measure))
    {
        object->_parent->dirty |= (NTG_OBJECT_DIRTY_MEASURE |
                NTG_OBJECT_DIRTY_CONSTRAIN);
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
        layout_ops.constrain_fn(object, object->__ldata, orient, _sizes, arena);

        size_t old_size, it_size;
        for(i = 0; i < children->size; i++)
        {
            it_size = ntg_object_size_map_get(_sizes, children->data[i]);
            if(orient == NTG_ORIENT_H)
            {
                old_size = children->data[i]->_size.x;
                children->data[i]->_size.x = it_size;
            }
            else
            {
                old_size = children->data[i]->_size.y;
                children->data[i]->_size.y = it_size;
            }
            if(old_size != it_size)
            {
                children->data[i]->dirty = (NTG_OBJECT_DIRTY_MEASURE |
                NTG_OBJECT_DIRTY_CONSTRAIN | NTG_OBJECT_DIRTY_ARRANGE |
                NTG_OBJECT_DIRTY_DRAW);
            }
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
        layout_ops.arrange_fn(object, object->__ldata, _poss, arena);

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
        layout_ops.draw_fn(object, object->__ldata, &_drawing, arena);

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
    return ntg_object_map_to_scene_space(object, ntg_xy(0, 0));
}

struct ntg_xy 
ntg_object_get_pos_in_space(const ntg_object* object, const ntg_object* space)
{
    assert(object != NULL);
    assert(space != NULL);

    if(object == space) return ntg_xy(0, 0);
    assert(ntg_object_is_descendant(space, object));

    struct ntg_xy out = ntg_xy(0, 0);

    const ntg_object* it = object;
    while(it != NULL && it != space)
    {
        out = ntg_xy_add(out, it->_pos);
        it = it->_parent;
    }

    return out;
}

size_t ntg_object_get_for_size(
        const ntg_object* object,
        ntg_orient orient,
        bool constrained)
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

void ntg_object_set_z_index(ntg_object* object, int z_index)
{
    assert(object != NULL);

    object->_z_index = z_index;
}

void ntg_object_add_dirty(ntg_object* object, uint8_t dirty)
{
    assert(object != NULL);

    object->dirty |= dirty;
}

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_object* object);

void ntg_object_init(ntg_object* object,
        struct ntg_object_layout_ops layout_ops,
        ntg_object_type type)
{
    assert(object != NULL);

    init_default(object);

    object->_type = type;

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
    object->__ldata = NULL;
    object->dirty = NTG_OBJECT_DIRTY_MEASURE | NTG_OBJECT_DIRTY_CONSTRAIN |
    NTG_OBJECT_DIRTY_ARRANGE | NTG_OBJECT_DIRTY_DRAW;

    object->_min_size = ntg_xy(0, 0);
    object->_max_size = ntg_xy(0, 0);
    object->_nat_size = ntg_xy(0, 0);
    object->_grow = ntg_xy(1, 1);
    object->_size = ntg_xy(0, 0);
    object->_pos = ntg_xy(0, 0);
    object->__ldata = NULL;
    object->_z_index = 0;
}

void ntg_object_deinit(ntg_object* object)
{
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

    if(parent->_scene)
    {
        object_register_tree(child, parent->_scene);
    }

    struct ntg_event_object_chldadd_data data1 = { .child = child };
    ntg_entity_raise_event_((ntg_entity*)parent, NTG_EVENT_OBJECT_CHLDADD, &data1);

    struct ntg_event_object_prntchng_data data2 = {
        .old = NULL,
        .new = parent
    };
    ntg_entity_raise_event_((ntg_entity*)child, NTG_EVENT_OBJECT_PRNTCHNG, &data2);

    ntg_object_add_dirty(parent, NTG_OBJECT_DIRTY_FULL);
}

void ntg_object_detach(ntg_object* object)
{
    assert(object != NULL);

    ntg_object* parent = object->_parent;
    if(parent == NULL) return;

    ntg_object_vec_rm(&parent->_children, object, NULL);

    object->_parent = NULL;

    if(object->_scene)
        object_unregister_tree(object, object->_scene);

    struct ntg_event_object_chldrm_data data1 = { .child = object };
    ntg_entity_raise_event_((ntg_entity*)parent, NTG_EVENT_OBJECT_CHLDRM, &data1);

    struct ntg_event_object_prntchng_data data2 = {
        .old = parent,
        .new = NULL
    };
    ntg_entity_raise_event_((ntg_entity*)object, NTG_EVENT_OBJECT_PRNTCHNG, &data2);

    ntg_object_add_dirty((ntg_object*)parent, NTG_OBJECT_DIRTY_FULL);
}

void _ntg_object_attach_scene(ntg_object* root, ntg_scene* scene)
{
    assert(root);
    assert(!root->_parent);

    object_register_tree(root, scene);
}

void _ntg_object_detach_scene(ntg_object* root)
{
    assert(root);
    assert(!root->_parent);

    object_unregister_tree(root, NULL);
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
        object->__ldata = layout_ops.init_fn(object);
    }
    else
    {
        layout_ops.deinit_fn(object->__ldata, object);
        object->__ldata = NULL;
    }

    struct ntg_event_object_scnchng_data data = {
        .old = old,
        .new = scene
    };
    ntg_entity_raise_event_((ntg_entity*)object, NTG_EVENT_OBJECT_SCNCHNG, &data);
}
