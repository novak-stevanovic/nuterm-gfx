#include "ntg.h"
#include <assert.h>
#include <stdlib.h>
#include "shared/_uthash.h"

#define DEBUG 1

struct object_data
{
    bool measure1, constrain1, measure2, constrain2, arrange, draw;
    struct ntg_object_measure old_hmeasure, old_vmeasure;
    struct ntg_xy old_size, old_pos;
};

struct object_data_hh
{
    ntg_object* key;
    struct object_data data;
    UT_hash_handle hh;
};

static void scene_map_add(ntg_def_scene* scene, ntg_object* object);
static struct object_data* scene_map_get(ntg_def_scene* scene, ntg_object* object);
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

    if(->_root == NULL)
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

    ntg_object* root = ntg_widget_get_group_root_(_scene->_root);
    struct object_data* root_data = scene_map_get(scene, root);
    assert(root_data != NULL);
    ntg_object_root_layout(root, size);

    if(!(ntg_xy_are_equal(root_data->old_size, size)))
    {
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
        .draw = true,
        .old_hmeasure = (struct ntg_object_measure) {0},
        .old_vmeasure = (struct ntg_object_measure) {0},
        .old_size = ntg_xy(0, 0),
        .old_pos = ntg_xy(0, 0)
    };

    struct object_data_hh* head = (struct object_data_hh*)scene->__map;
    HASH_ADD_PTR(head, key, data_hh);
    scene->__map = head;
}

static struct object_data* scene_map_get(ntg_def_scene* scene, ntg_object* object)
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
    struct object_data* object_data = scene_map_get(scene, object);

    if(!object_data->measure1) return;

    ntg_object* parent = object->_parent;
    if(parent == NULL)
    {
        object_data->measure1 = false;
        object_data->old_hmeasure = (struct ntg_object_measure) {0};
        return;
    }
    struct object_data* parent_data = scene_map_get(scene, parent);

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M1 | %p", object);

    ntg_object_measure(object, NTG_ORIENT_H, NTG_SIZE_MAX, arena);

    struct ntg_object_measure new_measure;
    new_measure = ntg_object_get_measure(object, NTG_ORIENT_H);

    struct ntg_object_measure old_measure;
    old_measure = object_data->old_hmeasure;

    if(!(ntg_object_measure_are_equal(old_measure, new_measure)))
    {
        parent_data->measure1 = true;
        parent_data->measure2 = true;
        parent_data->constrain1 = true;
        parent_data->constrain2 = true;
        object_data->old_hmeasure = new_measure;
    }

    object_data->measure1 = false;
}

static void constrain1_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get(scene, object);
    const ntg_object_vec* children = object->_children;

    if(!object_data->constrain1) return;

    if(children->size == 0)
    {
        object_data->constrain1 = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C1 | %p", object);

    ntg_object_constrain(object, NTG_ORIENT_H, size, arena);

    size_t i;
    ntg_object* it_child;
    struct object_data* it_child_data;
    struct ntg_xy it_old_size;
    size_t it_size;
    for(i = 0; i < children->size; i++)
    {
        it_child = children->data[i];
        it_child_data = scene_map_get(scene, it_child);
        it_old_size = it_child_data.old_size.x;
        it_size = it_child->_size.x;

        if(it_old_size != it_size)
        {
            it_child_data->constrain1 = true;
            it_child_data->measure2 = true;
            it_child_data->constrain2 = true;
            it_child_data->arrange = true;
            it_child_data->draw = true;
            object_data->arrange = true;
            it_child_data->old_size = it_size;
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
    struct object_data* object_data = scene_map_get(scene, object);

    if(!object_data->measure2) return;

    ntg_object* parent = object->_parent;
    if(parent == NULL)
    {
        object_data->measure2 = false;
        object_data->old_vmeasure = (struct ntg_object_measure) {0};
        return;
    }
    struct object_data* parent_data = scene_map_get(scene, parent);

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M2 | %p", object);

    ntg_object_measure(object, NTG_ORIENT_V, object->_size.x, arena);

    struct ntg_object_measure new_measure;
    new_measure = ntg_object_get_measure(object, NTG_ORIENT_V);

    struct ntg_object_measure old_measure;
    old_measure = object_data->old_hmeasure;

    if(!(ntg_object_measure_are_equal(old_measure, new_measure)))
    {
        parent_data->measure2 = true;
        parent_data->constrain2 = true;
        object_data->old_vmeasure = new_measure;
    }

    object_data->measure2 = false;
}

static void constrain2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get(scene, object);
    const ntg_object_vec* children = object->_children;

    if(!object_data->constrain2) return;

    if(children->size == 0)
    {
        object_data->constrain2 = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C2 | %p", object);

    ntg_object_constrain(object, NTG_ORIENT_V, object->_size.y, arena);

    size_t i;
    ntg_object* it_child;
    struct object_data* it_child_data;
    struct ntg_xy it_old_size;
    size_t it_size;
    for(i = 0; i < children->size; i++)
    {
        it_child = children->data[i];
        it_child_data = scene_map_get(scene, it_child);
        it_old_size = it_child_data.old_size.y;
        it_size = it_child->_size.y;

        if(it_old_size != it_size)
        {
            it_child_data->constrain2 = true;
            it_child_data->arrange = true;
            it_child_data->draw = true;
            object_data->arrange = true;
            it_child_data->old_size.y = it_size;
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
    struct object_data* object_data = scene_map_get(scene, object);
    const ntg_object_vec* children = object->_children;

    if(!object_data->arrange) return;

    if(children->size == 0)
    {
        object_data->arrange = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | A | %p", object);

    ntg_object_arrange(object, object->_size, arena);

    size_t i;
    ntg_object* it_child;
    struct object_data* it_child_data;
    struct ntg_xy it_old_pos, it_pos;
    for(i = 0; i < children->size; i++)
    {
        it_child = children->data[i];
        it_child_data = scene_map_get(scene, it_child);
        it_old_pos = it_child_data->old_pos;
        it_pos = it_child->_position;

        it_child_data->old_pos = it_pos;
    }

    object_data->arrange = false;
}

static void draw_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    struct object_data* object_data = scene_map_get(scene, object);

    if(!object_data->draw) return;

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | D | %p", object);

    ntg_object_draw(object, arena);

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

        struct object_data* data = scene_map_get(scene, source_object);

        data->measure1 = true,
        data->constrain1 = true,
        data->measure2 = true,
        data->constrain2 = true,
        data->arrange = true,
        data->draw = true
    }
}
