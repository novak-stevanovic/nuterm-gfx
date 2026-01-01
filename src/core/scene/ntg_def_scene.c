#include <stdlib.h>
#include <assert.h>

#include "core/scene/ntg_def_scene.h"
#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_measure_map.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "core/scene/ntg_scene_graph.h"
#include "shared/ntg_log.h"
#include "shared/sarena.h"

static void scene_observe_fn(ntg_entity* observer, struct ntg_event event);

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

    _ntg_scene_init_((ntg_scene*)scene, focuser, _ntg_def_scene_layout_fn);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

static ntg_object_measure_map* get_object_measure_map(ntg_object* object,
        ntg_def_scene* scene, ntg_orientation orientation, sarena* arena);
// static ntg_object_size_map* get_object_size_map(ntg_object* object,
//         ntg_def_scene* scene, ntg_orientation orientation);
static ntg_object_xy_map* get_object_size_map_xy(ntg_object* object,
        ntg_def_scene* scene, sarena* arena);
static ntg_object_xy_map* get_object_pos(ntg_object* object,
        ntg_def_scene* scene, sarena* arena);

static void measure1_fn(ntg_object* object, void* _layout_data);
static void constrain1_fn(ntg_object* object, void* _layout_data);
static void measure2_fn(ntg_object* object, void* _layout_data);
static void constrain2_fn(ntg_object* object, void* _layout_data);
static void arrange_fn(ntg_object* object, void* _layout_data);
static void draw_fn(ntg_object* object, void* _layout_data);

struct ntg_layout_data
{
    ntg_def_scene* scene;
    sarena* arena;
};

void _ntg_def_scene_deinit_fn(ntg_entity* entity)
{
    _ntg_scene_deinit_fn(entity);
}

void _ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size)
{
    assert(_scene != NULL);

    if(_scene->_root == NULL) return;

    ntg_object* true_root = ntg_object_get_group_root_(_scene->_root);

    ntg_def_scene* scene = (ntg_def_scene*)_scene;

    sarena* arena = sarena_create(50000);
    assert(arena != NULL);

    struct ntg_layout_data data = {
        .scene = scene,
        .arena = arena
    };

    struct ntg_scene_node_protected* root_data = ntg_scene_graph_get(_ntg_scene_get_graph(_scene), true_root);

    root_data->size = size;
    root_data->position = ntg_xy(0, 0);

    ntg_object_tree_perform(true_root, NTG_OBJECT_PERFORM_BOTTOM_UP, measure1_fn, &data);

    ntg_object_tree_perform(true_root, NTG_OBJECT_PERFORM_TOP_DOWN, constrain1_fn, &data);

    ntg_object_tree_perform(true_root, NTG_OBJECT_PERFORM_BOTTOM_UP, measure2_fn, &data);

    ntg_object_tree_perform(true_root, NTG_OBJECT_PERFORM_TOP_DOWN, constrain2_fn, &data);

    ntg_object_tree_perform(true_root, NTG_OBJECT_PERFORM_TOP_DOWN, arrange_fn, &data);

    ntg_object_tree_perform(true_root, NTG_OBJECT_PERFORM_TOP_DOWN, draw_fn, &data);

    sarena_destroy(arena);
}

/* -------------------------------------------------------------------------- */

static ntg_object_measure_map* get_object_measure_map(ntg_object* object,
        ntg_def_scene* scene, ntg_orientation orientation, sarena* arena)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    // struct _ntg_scene_node* node = ntg_scene_graph_get(graph, object);
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_measure_map* measures = ntg_object_measure_map_new(
            child_count, arena);

    ntg_object* it_child;
    struct ntg_scene_node_protected* it_node;
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

// static ntg_object_size_map* __get_object_size_map_size(ntg_object* object,
//         ntg_def_scene* scene, ntg_orientation orientation)
// {
//     ntg_scene* _scene = (ntg_scene*)scene;
//     ntg_scene_graph* graph = ntg_scene_get_graph(_scene);
//
//     struct _ntg_scene_node* node = ntg_scene_graph_get(graph, object);
//     const ntg_object_vec* children = ntg_object_get_children(object);
//     size_t child_count = children->_count;
//
//     ntg_object_size_map* sizes = ntg_object_size_map_new(
//             child_count, arena);
//
//     ntg_object* it_child;
//     struct _ntg_scene_node* it_node;
//     size_t it_size;
//     size_t i;
//     for(i = 0; i < child_count; i++)
//     {
//         it_child = children->_data[i];
//         it_node = ntg_scene_graph_get(graph, it_child);
//
//         it_size = (orientation == NTG_ORIENTATION_H) ?
//             it_node->size.x : it_node->size.y;
//
//         ntg_object_size_map_set(sizes, it_child, it_size);
//     }
//
//     return sizes;
// }

static ntg_object_xy_map* get_object_size_map_xy(ntg_object* object,
        ntg_def_scene* scene, sarena* arena)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    // struct _ntg_scene_node* node = ntg_scene_graph_get(graph, object);
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_xy_map* sizes = ntg_object_xy_map_new(
            child_count, arena);

    ntg_object* it_child;
    struct ntg_scene_node_protected* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        ntg_object_xy_map_set(sizes, it_child, it_node->size);
    }

    return sizes;
}

static ntg_object_xy_map* get_object_pos(ntg_object* object,
        ntg_def_scene* scene, sarena* arena)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    // struct _ntg_scene_node* node = ntg_scene_graph_get(graph, object);
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_xy_map* positions = ntg_object_xy_map_new(
            child_count, arena);

    ntg_object* it_child;
    struct ntg_scene_node_protected* it_node;
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
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    sarena* arena = data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_scene_node_protected* node = ntg_scene_graph_get(graph, object);
    // const ntg_object_vec* children = ntg_object_get_children(object);
    // size_t child_count = children->_count;

    ntg_object_measure_map* measures = get_object_measure_map(object,
            scene, NTG_ORIENTATION_H, arena);

    struct ntg_object_measure_ctx ctx = { .measures = measures };
    struct ntg_object_measure_out out = {0};

    struct ntg_object_measure result = ntg_object_measure(
            object, NTG_ORIENTATION_H,
            NTG_SIZE_MAX, ctx, &out, node->object_layout_data,
            arena);

    node->min_size.x = result.min_size;
    node->natural_size.x = result.natural_size;
    node->max_size.x = result.max_size;
    node->grow.x = result.grow;
}

static void constrain1_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    sarena* arena = data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_scene_node_protected* node = ntg_scene_graph_get(graph, object);
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_measure_map* measures = get_object_measure_map(object,
            scene, NTG_ORIENTATION_H, arena);
    ntg_object_size_map* _sizes = ntg_object_size_map_new(child_count,
            arena);

    struct ntg_object_constrain_ctx ctx = { .measures = measures };
    struct ntg_object_constrain_out out = { .sizes = _sizes };

    ntg_object_constrain(object,
            NTG_ORIENTATION_H,
            node->size.x, ctx,
            &out,
            node->object_layout_data,
            arena);

    size_t i;
    const ntg_object* it_child;
    struct ntg_scene_node_protected* it_node;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        it_node->size.x = ntg_object_size_map_get(out.sizes, it_child);
    }
}

static void measure2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    sarena* arena = data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_scene_node_protected* node = ntg_scene_graph_get(graph, object);

    ntg_object_measure_map* measures = get_object_measure_map(object,
            scene, NTG_ORIENTATION_V, arena);

    struct ntg_object_measure_ctx ctx = { .measures = measures };
    struct ntg_object_measure_out out = {0};

    struct ntg_object_measure result = ntg_object_measure(
            object, NTG_ORIENTATION_V,
            node->size.x, ctx, &out,
            node->object_layout_data,
            arena);

    node->min_size.y = result.min_size;
    node->natural_size.y = result.natural_size;
    node->max_size.y = result.max_size;
    node->grow.y = result.grow;
}

static void constrain2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    sarena* arena = data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_scene_node_protected* node = ntg_scene_graph_get(graph, object);
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_measure_map* measures = get_object_measure_map(object,
            scene, NTG_ORIENTATION_V, arena);
    ntg_object_size_map* _sizes = ntg_object_size_map_new(child_count,
            arena);

    struct ntg_object_constrain_ctx ctx = { .measures = measures };
    struct ntg_object_constrain_out out = { .sizes = _sizes };

    ntg_object_constrain(object,
            NTG_ORIENTATION_V,
            node->size.y, ctx,
            &out,
            node->object_layout_data,
            arena);

    size_t i;
    const ntg_object* it_child;
    struct ntg_scene_node_protected* it_node;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        it_node->size.y = ntg_object_size_map_get(out.sizes, it_child);
    }
}

static void arrange_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    sarena* arena = data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_scene_node_protected* node = ntg_scene_graph_get(graph, object);
    struct ntg_object_vecv children = ntg_object_get_children_(object);
    size_t child_count = children.count;

    ntg_object_xy_map* sizes = get_object_size_map_xy(object, scene, arena);
    ntg_object_xy_map* _positions = ntg_object_xy_map_new(child_count, arena);

    size_t i;
    struct ntg_scene_node_protected* it_node;
    ntg_object* it_child;
    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        ntg_object_xy_map_set(sizes, it_child, it_node->size);
    }

    struct ntg_object_arrange_ctx ctx = { .sizes = sizes };
    struct ntg_object_arrange_out out = { .positions = _positions };

    ntg_object_arrange(object, node->size, ctx, &out,
            node->object_layout_data,
            arena);

    for(i = 0; i < child_count; i++)
    {
        it_child = children.data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        it_node->position = ntg_xy_add(
                node->position,
                ntg_object_xy_map_get(_positions, it_child));
    }
}

static void draw_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    sarena* arena = data->arena; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct ntg_scene_node_protected* node = ntg_scene_graph_get(graph, object);
    ntg_object_drawing_set_size(node->drawing, node->size);

    ntg_object_xy_map* sizes = get_object_size_map_xy(object, scene, arena);
    ntg_object_xy_map* positions = get_object_pos(object, scene, arena);

    struct ntg_object_draw_ctx ctx = { .sizes = sizes, .positions = positions };
    struct ntg_object_draw_out out = { .drawing = node->drawing };

    ntg_object_draw(object, node->size, ctx, &out,
            node->object_layout_data, arena);
}
