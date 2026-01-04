#include "ntg.h"
#include "core/scene/ntg_def_scene.h"
#include "base/entity/ntg_entity_type.h"
#include "base/entity/ntg_event_type.h"
#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_measure.h"
#include "core/object/shared/ntg_object_measure_map.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "core/scene/ntg_scene_graph.h"
#include "shared/ntg_log.h"

#define DEBUG 0

struct node_data
{
    bool measure1, constrain1, measure2, constrain2, arrange, draw;
};

static ntg_object_measure_map* get_children_measures(ntg_object* object,
        ntg_def_scene* scene, ntg_orientation orientation, sarena* arena);
static ntg_object_xy_map* get_children_sizes_xy(ntg_object* object,
        ntg_def_scene* scene, sarena* arena);
static ntg_object_xy_map* get_children_positions(ntg_object* object,
        ntg_def_scene* scene, sarena* arena);

static void measure1_fn(ntg_object* object, void* _layout_data);
static void constrain1_fn(ntg_object* object, void* _layout_data);
static void measure2_fn(ntg_object* object, void* _layout_data);
static void constrain2_fn(ntg_object* object, void* _layout_data);
static void arrange_fn(ntg_object* object, void* _layout_data);
static void draw_fn(ntg_object* object, void* _layout_data);

static void* graph_node_data_init_fn(ntg_scene* _scene, ntg_object* object);
static void graph_node_data_deinit_fn(ntg_scene* _scene, ntg_object* object, void* data);
static void observe_fn(ntg_entity* entity, struct ntg_event event);

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

void _ntg_def_scene_init_(ntg_def_scene* scene, ntg_focuser* focuser)
{
    assert(scene != NULL);

    struct ntg_scene_hooks hooks = {
        .graph_node_data_init_fn = graph_node_data_init_fn,
        .graph_node_data_deinit_fn = graph_node_data_deinit_fn
    };

    _ntg_scene_init_((ntg_scene*)scene, focuser, _ntg_def_scene_layout_fn, hooks);

    scene->__detected_changes = true;
    scene->__last_size = ntg_xy(0, 0);
    
    ntg_entity_observe((ntg_entity*)scene, (ntg_entity*)scene, observe_fn);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */


struct ntg_layout_data
{
    ntg_def_scene* scene;
    sarena* arena;
};

void _ntg_def_scene_deinit_fn(ntg_entity* entity)
{
    ntg_def_scene* scene = (ntg_def_scene*)entity;

    scene->__detected_changes = true;
    scene->__last_size = ntg_xy(0, 0);

    _ntg_scene_deinit_fn(entity);
}

void _ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size)
{
    assert(_scene != NULL);

    ntg_def_scene* scene = (ntg_def_scene*)_scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    if(_scene->_root == NULL)
    {
        scene->__last_size = size;
        scene->__detected_changes = false;
        return;
    }

    if(ntg_xy_are_equal(scene->__last_size, size) && !scene->__detected_changes)
        return;

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE: Layout unoptimized");

    ntg_object* root = ntg_object_get_group_root_(_scene->_root);
    struct ntg_scene_node_pr* root_node = ntg_scene_graph_get(graph, root);

    sarena* arena = sarena_create(50000);
    assert(arena != NULL);

    struct ntg_layout_data data = {
        .scene = scene,
        .arena = arena
    };

    root_node->position = ntg_xy(0, 0);
    if(!ntg_xy_are_equal(root_node->size, size))
    {
        root_node->size = size;
        struct node_data* root_node_data = root_node->data;
        root_node_data->constrain1 = true;
        root_node_data->constrain2 = true;
        root_node_data->arrange = true;
        root_node_data->draw = true;
    }

    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_BOTTOM_UP, measure1_fn, &data);
    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN, constrain1_fn, &data);
    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_BOTTOM_UP, measure2_fn, &data);
    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN, constrain2_fn, &data);
    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN, arrange_fn, &data);
    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN, draw_fn, &data);

    sarena_destroy(arena);

    scene->__last_size = size;
    scene->__detected_changes = false;
}

/* -------------------------------------------------------------------------- */

static ntg_object_measure_map* get_children_measures(ntg_object* object,
        ntg_def_scene* scene, ntg_orientation orientation, sarena* arena)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_measure_map* measures = ntg_object_measure_map_new(child_count, arena);

    ntg_object* it_child;
    struct ntg_scene_node_pr* it_node;
    struct ntg_object_measure it_measure;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        it_measure = (struct ntg_object_measure) {
            .min_size = (orientation == NTG_ORIENTATION_H) ?
                it_node->min_size.x : it_node->min_size.y,
            .natural_size = (orientation == NTG_ORIENTATION_H) ? 
                it_node->natural_size.x : it_node->natural_size.y,
            .max_size = (orientation == NTG_ORIENTATION_H) ? 
                it_node->max_size.x : it_node->max_size.y,
            .grow = (orientation == NTG_ORIENTATION_H) ? 
                it_node->grow.x : it_node->grow.y
        };

        ntg_object_measure_map_set(measures, it_child, it_measure);
    }

    return measures;
}

static ntg_object_xy_map* get_children_sizes_xy(ntg_object* object,
        ntg_def_scene* scene, sarena* arena)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_xy_map* sizes = ntg_object_xy_map_new(child_count, arena);

    ntg_object* it_child;
    struct ntg_scene_node_pr* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        ntg_object_xy_map_set(sizes, it_child, it_node->size);
    }

    return sizes;
}

static ntg_object_xy_map* get_children_positions(ntg_object* object,
        ntg_def_scene* scene, sarena* arena)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_xy_map* positions = ntg_object_xy_map_new(child_count, arena);

    ntg_object* it_child;
    struct ntg_scene_node_pr* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        ntg_object_xy_map_set(positions, it_child, it_node->position);
    }

    return positions;
}

static void measure1_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);
    struct ntg_scene_node_pr* node = ntg_scene_graph_get(graph, object);
    struct node_data* node_data = node->data;

    if(!node_data->measure1) return;

    ntg_object* parent = ntg_object_get_parent_(object, true);
    if(parent == NULL)
    {
        node_data->measure1 = false;
        return;
    }
    struct ntg_scene_node_pr* parent_node = ntg_scene_graph_get(graph, parent);
    struct node_data* parent_node_data = parent_node->data;

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M1 | %p", object);

    ntg_object_measure_map* measures = get_children_measures(object,
            scene, NTG_ORIENTATION_H, arena);

    struct ntg_object_measure_ctx ctx = { .measures = measures };
    struct ntg_object_measure_out out = {0};

    struct ntg_object_measure result = ntg_object_measure(object,
            NTG_ORIENTATION_H, NTG_SIZE_MAX, ctx, &out,
            node->object_layout_data, arena);

    if((node->min_size.x != result.min_size) ||
            (node->natural_size.x != result.natural_size) ||
            (node->max_size.x != result.max_size) ||
            (node->grow.x != result.grow))
    {
        parent_node_data->measure1 = true;
        parent_node_data->measure2 = true;
        parent_node_data->constrain1 = true;
        parent_node_data->constrain2 = true;
    }

    node->min_size.x = result.min_size;
    node->natural_size.x = result.natural_size;
    node->max_size.x = result.max_size;
    node->grow.x = result.grow;

    node_data->measure1 = false;
}

static void constrain1_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);
    struct ntg_scene_node_pr* node = ntg_scene_graph_get(graph, object);
    struct node_data* node_data = node->data;
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    if(!node_data->constrain1) return;

    if(child_count == 0)
    {
        node_data->constrain1 = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C1 | %p", object);

    ntg_object_measure_map* measures = get_children_measures(object,
            scene, NTG_ORIENTATION_H, arena);
    ntg_object_size_map* _sizes = ntg_object_size_map_new(child_count, arena);

    struct ntg_object_constrain_ctx ctx = { .measures = measures };
    struct ntg_object_constrain_out out = { .sizes = _sizes };

    ntg_object_constrain(object, NTG_ORIENTATION_H,
            node->size.x, ctx, &out,
            node->object_layout_data, arena);

    size_t i;
    const ntg_object* it_child;
    struct ntg_scene_node_pr* it_node;
    struct node_data* it_node_data;
    size_t it_size;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);
        it_node_data = it_node->data;
        it_size = ntg_object_size_map_get(out.sizes, it_child);

        if(it_node->size.x != it_size)
        {
            it_node->size.x = it_size;
            it_node_data->constrain1 = true;
            it_node_data->measure2 = true;
            it_node_data->constrain2 = true;
            it_node_data->arrange = true;
            it_node_data->draw = true;
            node_data->arrange = true;
        }
    }

    node_data->constrain1 = false;
}

static void measure2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);
    struct ntg_scene_node_pr* node = ntg_scene_graph_get(graph, object);
    struct node_data* node_data = node->data;

    if(!node_data->measure2) return;

    ntg_object* parent = ntg_object_get_parent_(object, true);
    if(parent == NULL)
    {
        node_data->measure2 = false;
        return;
    }
    struct ntg_scene_node_pr* parent_node = ntg_scene_graph_get(graph, parent);
    struct node_data* parent_node_data = parent_node->data;

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | M2 | %p", object);

    ntg_object_measure_map* measures = get_children_measures(object,
            scene, NTG_ORIENTATION_V, arena);

    struct ntg_object_measure_ctx ctx = { .measures = measures };
    struct ntg_object_measure_out out = {0};

    struct ntg_object_measure result = ntg_object_measure(
            object, NTG_ORIENTATION_V,
            node->size.x, ctx, &out,
            node->object_layout_data,
            arena);

    if((node->min_size.y != result.min_size) ||
            (node->natural_size.y != result.natural_size) ||
            (node->max_size.y != result.max_size) ||
            (node->grow.y != result.grow))
    {
        parent_node_data->measure2 = true;
        parent_node_data->constrain2 = true;
    }

    node->min_size.y = result.min_size;
    node->natural_size.y = result.natural_size;
    node->max_size.y = result.max_size;
    node->grow.y = result.grow;

    node_data->measure2 = false;
}

static void constrain2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);
    struct ntg_scene_node_pr* node = ntg_scene_graph_get(graph, object);
    struct node_data* node_data = node->data;
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    if(!node_data->constrain2) return;

    if(child_count == 0)
    {
        node_data->constrain2 = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | C2 | %p", object);

    ntg_object_measure_map* measures = get_children_measures(object,
            scene, NTG_ORIENTATION_V, arena);
    ntg_object_size_map* _sizes = ntg_object_size_map_new(child_count,
            arena);

    struct ntg_object_constrain_ctx ctx = { .measures = measures };
    struct ntg_object_constrain_out out = { .sizes = _sizes };

    ntg_object_constrain(object, NTG_ORIENTATION_V,
            node->size.y, ctx, &out,
            node->object_layout_data, arena);

    size_t i;
    const ntg_object* it_child;
    struct ntg_scene_node_pr* it_node;
    struct node_data* it_node_data;
    size_t it_size;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);
        it_node_data = it_node->data;
        it_size = ntg_object_size_map_get(out.sizes, it_child);

        if(it_node->size.y != it_size)
        {
            it_node->size.y = it_size;
            it_node_data->constrain2 = true;
            it_node_data->arrange = true;
            it_node_data->draw = true;
            node_data->arrange = true;
        }
    }

    node_data->constrain2 = false;
}

static void arrange_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);
    struct ntg_scene_node_pr* node = ntg_scene_graph_get(graph, object);
    struct node_data* node_data = node->data;
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    if(!node_data->arrange) return;

    if(child_count == 0)
    {
        node_data->arrange = false;
        return;
    }

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | A | %p", object);

    ntg_object_xy_map* sizes = get_children_sizes_xy(object, scene, arena);
    ntg_object_xy_map* _positions = ntg_object_xy_map_new(child_count, arena);

    struct ntg_object_arrange_ctx ctx = { .sizes = sizes };
    struct ntg_object_arrange_out out = { .positions = _positions };

    ntg_object_arrange(object, node->size, ctx, &out,
            node->object_layout_data, arena);

    size_t i;
    const ntg_object* it_child;
    struct ntg_scene_node_pr* it_node;
    struct ntg_xy it_pos;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);
        it_pos = ntg_object_xy_map_get(_positions, it_child);

        it_node->position = it_pos;
    }

    node_data->arrange = false;
}

static void draw_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* layout_data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = layout_data->scene; 
    sarena* arena = layout_data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);
    struct ntg_scene_node_pr* node = ntg_scene_graph_get(graph, object);
    struct node_data* node_data = node->data;

    if(!node_data->draw) return;

    if(DEBUG) ntg_log_log("NTG_DEF_SCENE | D | %p", object);

    ntg_object_drawing_set_size(node->drawing, node->size);

    ntg_object_xy_map* sizes = get_children_sizes_xy(object, scene, arena);
    ntg_object_xy_map* positions = get_children_positions(object, scene, arena);

    struct ntg_object_draw_ctx ctx = { .sizes = sizes, .positions = positions };
    struct ntg_object_draw_out out = { .drawing = node->drawing };

    ntg_object_draw(object, node->size, ctx, &out,
            node->object_layout_data, arena);

    node_data->draw = false;
}

static void* graph_node_data_init_fn(ntg_scene* _scene, ntg_object* object)
{
    struct node_data* data = malloc(sizeof(struct node_data));

    (*data) = (struct node_data) {
        .measure1 = true,
        .constrain1 = true,
        .measure2 = true,
        .constrain2 = true,
        .arrange = true,
        .draw = true
    };

    return data;
}

static void graph_node_data_deinit_fn(ntg_scene* _scene, ntg_object* object, void* data)
{
    free(data);
}

static void observe_fn(ntg_entity* entity, struct ntg_event event)
{
    ntg_scene* _scene = (ntg_scene*)entity;
    ntg_def_scene* scene = (ntg_def_scene*)entity;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    if(event.type == NTG_EVENT_SCENE_OBJADD)
    {
        struct ntg_event_scene_objadd_data* data = event.data;
        ntg_object* added = data->object;
        // graph node data init is already handled by the hook fn
        scene->__detected_changes = true;

        ntg_entity_observe(entity, (ntg_entity*)added, observe_fn);
    }
    else if(event.type == NTG_EVENT_SCENE_OBJRM)
    {
        struct ntg_event_scene_objrm_data* data = event.data;
        ntg_object* removed = data->object;
        // graph node data deinit is already handled by the hook fn
        scene->__detected_changes = true;

        ntg_entity_stop_observing(entity, (ntg_entity*)removed, observe_fn);
    }
    else if(event.type == NTG_EVENT_OBJECT_CHANGE)
    {
        scene->__detected_changes = true;

        ntg_object* source_object = (ntg_object*)event.source;

        struct ntg_scene_node_pr* node = ntg_scene_graph_get(graph, source_object);
        struct node_data* node_data = node->data;

        (*node_data) = (struct node_data) {
            .measure1 = true,
            .constrain1 = true,
            .measure2 = true,
            .constrain2 = true,
            .arrange = true,
            .draw = true
        };
    }
}
