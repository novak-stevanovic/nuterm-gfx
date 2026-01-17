#include "ntg.h"
#include <assert.h>
#include <stdlib.h>
#include "shared/_uthash.h"

#define DEBUG 1

struct object_data
{
    bool measure1, constrain1, measure2, constrain2, arrange, draw;
};

struct object_data_hh
{
    ntg_object* key;
    struct object_data data;
    UT_hash_handle hh;
};

static void scene_map_add(ntg_def_scene* scene, ntg_object* object);
static struct object_data* scene_map_get_(ntg_def_scene* scene, ntg_object* object);
static void scene_map_del(ntg_def_scene* scene, ntg_object* object);
static void scene_map_del_all(ntg_def_scene* scene);

static void observe_fn(ntg_entity* entity, struct ntg_event event);

static void measure1_fn(ntg_object* object, void* _layout_data);
static void constrain1_fn(ntg_object* object, void* _layout_data);
static void measure2_fn(ntg_object* object, void* _layout_data);
static void constrain2_fn(ntg_object* object, void* _layout_data);
static void arrange_fn(ntg_object* object, void* _layout_data);
static void draw_fn(ntg_object* object, void* _layout_data);

NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(measure1_tree, measure1_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(constrain1_tree, constrain1_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(measure2_tree, measure2_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(constrain2_tree, constrain2_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(arrange_tree, arrange_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(draw_tree, draw_fn);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_scene* ntg_def_scene_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .deinit_fn = _ntg_def_scene_deinit_fn,
        .type = &NTG_ENTITY_DEF_SCENE,
        .system = system
    };

    ntg_def_scene* new = (ntg_def_scene*)ntg_entity_create(entity_data);

    return new;
}

void ntg_def_scene_init(ntg_def_scene* scene)
{
    assert(scene != NULL);

    _ntg_scene_init((ntg_scene*)scene, _ntg_def_scene_layout_fn);

    scene->__detected_changes = true;
    scene->__last_size = ntg_xy(0, 0);
    scene->__map = NULL;
    
    ntg_entity_observe((ntg_entity*)scene, (ntg_entity*)scene, observe_fn);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_layout_data
{
    ntg_def_scene* scene;
    sarena* arena;
    struct ntg_xy size;
};

void _ntg_def_scene_deinit_fn(ntg_entity* entity)
{
    ntg_def_scene* scene = (ntg_def_scene*)entity;

    scene_map_del_all(scene);

    scene->__detected_changes = true;
    scene->__last_size = ntg_xy(0, 0);
    scene->__map = NULL;

    _ntg_scene_deinit_fn(entity);
}

void _ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size, sarena* arena)
{
    assert(_scene != NULL);

    ntg_def_scene* scene = (ntg_def_scene*)_scene;

    if(_scene->_root == NULL)
    {
        scene->__last_size = size;
        scene->__detected_changes = false;
        return;
    }

    if(ntg_xy_are_equal(scene->__last_size, size) && !(scene->__detected_changes))
        return;

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE: Layout unoptimized");

    struct ntg_layout_data data = {
        .scene = scene,
        .arena = arena,
        .size = size
    };

    ntg_object* root = ntg_object_get_group_root_(_scene->_root);
    ntg_object_set_position(root, ntg_xy(0, 0), NTG_OBJECT_SELF);

    struct ntg_xy root_size = ntg_object_get_size(root, NTG_OBJECT_SELF);
    if(!(ntg_xy_are_equal(root_size, size)))
    {
        ntg_object_set_size(root, size, NTG_OBJECT_SELF);
        struct object_data* root_data = scene_map_get_(scene, root);
        assert(root_data != NULL);
        root_data->constrain1 = true;
        root_data->constrain2 = true;
        root_data->arrange = true;
        root_data->draw = true;
    }

    measure1_tree(root, &data);
    constrain1_tree(root, &data);
    measure2_tree(root, &data);
    constrain2_tree(root, &data);
    arrange_tree(root, &data);
    draw_tree(root, &data);

    scene->__last_size = size;
    scene->__detected_changes = false;
}

/* -------------------------------------------------------------------------- */
/* PRIVATE */
/* -------------------------------------------------------------------------- */

static void scene_map_add(ntg_def_scene* scene, ntg_object* object)
{
    struct object_data_hh* data_hh = malloc(sizeof(struct object_data_hh));
    assert(data_hh != NULL);

    data_hh->key = object;
    data_hh->data = (struct object_data) {
        .measure1 = true,
        .constrain1 = true,
        .measure2 = true,
        .constrain2 = true,
        .arrange = true,
        .draw = true
    };

    struct object_data_hh* head = (struct object_data_hh*)scene->__map;
    HASH_ADD_PTR(head, key, data_hh);
    scene->__map = head;
}

static struct object_data* scene_map_get_(ntg_def_scene* scene, ntg_object* object)
{
    struct object_data_hh* head = (struct object_data_hh*)scene->__map;

    struct object_data_hh* found;
    HASH_FIND_PTR(head, &object, found);
    scene->__map = head;
    
    return (found != NULL) ? &found->data : NULL;
}

static void scene_map_del(ntg_def_scene* scene, ntg_object* object)
{
    struct object_data_hh* head = (struct object_data_hh*)scene->__map;

    struct object_data_hh* found;
    HASH_FIND_PTR(head, &object, found);
    if(found == NULL) return;

    HASH_DEL(head, found);
    free(found);

    scene->__map = head;
}

static void scene_map_del_all(ntg_def_scene* scene)
{
    struct object_data_hh* head = (struct object_data_hh*)scene->__map;

    struct object_data_hh *curr, *tmp;
    HASH_ITER(hh, head, curr, tmp)
    {
        HASH_DEL(head, curr);
        free(curr);
    }

    scene->__map = head;
}

static void measure1_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    // ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get_(scene, object);

    if(!object_data->measure1) return;

    ntg_object* parent = ntg_object_get_parent_(object, true);
    if(parent == NULL)
    {
        object_data->measure1 = false;
        return;
    }
    struct object_data* parent_data = scene_map_get_(scene, parent);

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M1 | %p", object);

    struct ntg_object_measure result = ntg_object_measure(object,
            NTG_ORIENT_H, NTG_SIZE_MAX, arena);

    struct ntg_object_measure old_measure = ntg_object_get_measure(object,
            NTG_ORIENT_H, NTG_OBJECT_SELF);

    if((old_measure.min_size != result.min_size) ||
            (old_measure.natural_size != result.natural_size) ||
            (old_measure.max_size != result.max_size) ||
            (old_measure.grow != result.grow))
    {
        parent_data->measure1 = true;
        parent_data->measure2 = true;
        parent_data->constrain1 = true;
        parent_data->constrain2 = true;

        ntg_object_set_measure(object, result, NTG_ORIENT_H, NTG_OBJECT_SELF);
    }

    object_data->measure1 = false;
}

static void constrain1_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get_(scene, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->size;

    if(!object_data->constrain1) return;

    if(child_count == 0)
    {
        object_data->constrain1 = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C1 | %p", object);

    ntg_object_size_map* _sizes = ntg_object_size_map_new(child_count, arena);
    size_t size = ntg_object_get_size(object, NTG_OBJECT_SELF).x;
    ntg_object_constrain(object, NTG_ORIENT_H, size, _sizes, arena);

    size_t i;
    ntg_object* it_child;
    struct object_data* it_object_data;
    struct ntg_xy it_old_size;
    size_t it_size;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->data[i];
        it_object_data = scene_map_get_(scene, it_child);
        it_old_size = ntg_object_get_size(it_child, NTG_OBJECT_SELF);
        it_size = ntg_object_size_map_get(_sizes, it_child, NTG_OBJECT_SELF);

        if(it_old_size.x != it_size)
        {
            ntg_object_set_size(it_child, ntg_xy(it_size, it_old_size.y), NTG_OBJECT_SELF);
            it_object_data->constrain1 = true;
            it_object_data->measure2 = true;
            it_object_data->constrain2 = true;
            it_object_data->arrange = true;
            it_object_data->draw = true;
            object_data->arrange = true;
        }
    }

    object_data->constrain1 = false;
}

static void measure2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get_(scene, object);

    if(!object_data->measure2) return;

    ntg_object* parent = ntg_object_get_parent_(object, true);
    if(parent == NULL)
    {
        object_data->measure2 = false;
        return;
    }
    struct object_data* parent_data = scene_map_get_(scene, parent);

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M2 | %p", object);

    size_t size = ntg_object_get_size(object, NTG_OBJECT_SELF).x;
    struct ntg_object_measure result = ntg_object_measure(object, NTG_ORIENT_V, size, arena);

    struct ntg_object_measure old_measure = ntg_object_get_measure(object,
            NTG_ORIENT_V, NTG_OBJECT_SELF);

    if((old_measure.min_size != result.min_size) ||
            (old_measure.natural_size != result.natural_size) ||
            (old_measure.max_size != result.max_size) ||
            (old_measure.grow != result.grow))
    {
        parent_data->measure2 = true;
        parent_data->constrain2 = true;

        ntg_object_set_measure(object, result, NTG_ORIENT_V, NTG_OBJECT_SELF);
    }

    object_data->measure2 = false;
}

static void constrain2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get_(scene, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->size;

    if(!object_data->constrain2) return;

    if(child_count == 0)
    {
        object_data->constrain2 = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C2 | %p", object);

    ntg_object_size_map* _sizes = ntg_object_size_map_new(child_count, arena);
    size_t size = ntg_object_get_size(object, NTG_OBJECT_SELF).y;
    ntg_object_constrain(object, NTG_ORIENT_V, size, _sizes, arena);

    size_t i;
    ntg_object* it_child;
    struct object_data* it_object_data;
    struct ntg_xy it_old_size;
    size_t it_size;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->data[i];
        it_object_data = scene_map_get_(scene, it_child);
        it_old_size = ntg_object_get_size(it_child, NTG_OBJECT_SELF);
        it_size = ntg_object_size_map_get(_sizes, it_child, NTG_OBJECT_SELF);

        if(it_old_size.y != it_size)
        {
            ntg_object_set_size(it_child, ntg_xy(it_old_size.x, it_size), NTG_OBJECT_SELF);
            it_object_data->constrain2 = true;
            it_object_data->arrange = true;
            it_object_data->draw = true;
            object_data->arrange = true;
        }
    }

    object_data->constrain2 = false;
}

static void arrange_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get_(scene, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->size;

    if(!object_data->arrange) return;

    if(child_count == 0)
    {
        object_data->arrange = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | A | %p", object);

    ntg_object_xy_map* _positions = ntg_object_xy_map_new(child_count, arena);

    struct ntg_xy size = ntg_object_get_size(object, NTG_OBJECT_SELF);
    ntg_object_arrange(object, size, _positions, arena);

    size_t i;
    ntg_object* it_child;
    struct ntg_xy it_pos;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->data[i];

        it_pos = ntg_object_xy_map_get(_positions, it_child, NTG_OBJECT_SELF);
        ntg_object_set_position(it_child, it_pos, NTG_OBJECT_SELF);
    }

    object_data->arrange = false;
}

static void draw_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get_(scene, object);

    if(!object_data->draw) return;

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | D | %p", object);

    struct ntg_xy size = ntg_object_get_size(object, NTG_OBJECT_SELF);

    ntg_temp_object_drawing _temp_drawing;
    ntg_temp_object_drawing_init(&_temp_drawing, size, arena);

    ntg_object_draw(object, size, &_temp_drawing, arena);

    ntg_object_update_drawing(object, &_temp_drawing, layout_data->size, NTG_OBJECT_SELF);

    object_data->draw = false;
}

static void observe_fn(ntg_entity* entity, struct ntg_event event)
{
    ntg_scene* _scene = (ntg_scene*)entity;
    ntg_def_scene* scene = (ntg_def_scene*)entity;

    if(event.type == NTG_EVENT_SCENE_OBJADD)
    {
        struct ntg_event_scene_objadd_data* data = event.data;
        ntg_object* added = data->object;
        scene->__detected_changes = true;

        scene_map_add(scene, added);

        ntg_entity_observe(entity, (ntg_entity*)added, observe_fn);
    }
    else if(event.type == NTG_EVENT_SCENE_OBJRM)
    {
        struct ntg_event_scene_objrm_data* data = event.data;
        ntg_object* removed = data->object;
        scene->__detected_changes = true;

        scene_map_del(scene, removed);

        ntg_entity_stop_observing(entity, (ntg_entity*)removed, observe_fn);
    }
    else if(event.type == NTG_EVENT_OBJECT_DIFF)
    {
        scene->__detected_changes = true;

        ntg_object* source_object = (ntg_object*)event.source;

        struct object_data* data = scene_map_get_(scene, source_object);

        (*data) = (struct object_data) {
            .measure1 = true,
            .constrain1 = true,
            .measure2 = true,
            .constrain2 = true,
            .arrange = true,
            .draw = true
        };
    }
}
