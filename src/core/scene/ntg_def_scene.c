#include <stdlib.h>
#include <assert.h>

#include "core/scene/ntg_def_scene.h"
#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_measure_map.h"
#include "core/object/shared/ntg_object_size_map.h"
#include "core/object/shared/ntg_object_xy_map.h"
#include "core/scene/_ntg_scene_graph.h"
#include "shared/sarena.h"

static ntg_object_measure_map* get_object_measure_map(ntg_object* object,
        ntg_def_scene* scene, ntg_orientation orientation);
// static ntg_object_size_map* get_object_size_map(ntg_object* object,
//         ntg_def_scene* scene, ntg_orientation orientation);
static ntg_object_xy_map* get_object_size_map_xy(ntg_object* object,
        ntg_def_scene* scene);
static ntg_object_xy_map* get_object_pos(ntg_object* object,
        ntg_def_scene* scene);

static void measure1_fn(ntg_object* object, void* _layout_data);
static void constrain1_fn(ntg_object* object, void* _layout_data);
static void measure2_fn(ntg_object* object, void* _layout_data);
static void constrain2_fn(ntg_object* object, void* _layout_data);
static void arrange_fn(ntg_object* object, void* _layout_data);
static void draw_fn(ntg_object* object, void* _layout_data);

void _ntg_def_scene_init_(
        ntg_def_scene* scene,
        ntg_scene_process_key_fn process_key_fn,
        void* data)
{
    assert(scene != NULL);

    _ntg_scene_init_((ntg_scene*)scene,
            _ntg_def_scene_layout_fn,
            NULL,
            NULL,
            process_key_fn,
            data);

    scene->__layout_arena = sarena_create(100000);
    assert(scene->__layout_arena != NULL);
}

void _ntg_def_scene_deinit_(ntg_def_scene* scene)
{
    assert(scene != NULL);

    _ntg_scene_deinit_((ntg_scene*)scene);

    sarena_destroy(scene->__layout_arena);
    scene->__layout_arena = NULL;
}

struct ntg_layout_data
{
    ntg_def_scene* scene;
};

void _ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size)
{
    assert(_scene != NULL);

    ntg_object* root = ntg_scene_get_root(_scene);
    if(_scene->_root == NULL) return;

    ntg_def_scene* scene = (ntg_def_scene*)_scene;

    struct ntg_layout_data data = {
        .scene = scene
    };

    struct ntg_scene_node_protect* root_data = ntg_scene_graph_get(_scene->_graph, root);

    root_data->size = size;
    root_data->position = ntg_xy(0, 0);

    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_BOTTOM_UP,
            measure1_fn, &data);

    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            constrain1_fn, &data);

    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_BOTTOM_UP,
            measure2_fn, &data);

    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            constrain2_fn, &data);

    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            arrange_fn, &data);

    ntg_object_tree_perform(root, NTG_OBJECT_PERFORM_TOP_DOWN,
            draw_fn, &data);

    sarena_rewind(scene->__layout_arena);
}

/* -------------------------------------------------------------------------- */

static ntg_object_measure_map* get_object_measure_map(ntg_object* object,
        ntg_def_scene* scene, ntg_orientation orientation)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    // struct _ntg_scene_node* node = ntg_scene_graph_get(graph, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->_count;

    ntg_object_measure_map* measures = ntg_object_measure_map_new(
            child_count, scene->__layout_arena);

    ntg_object* it_child;
    struct ntg_scene_node_protect* it_node;
    struct ntg_object_measure it_measure;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->_data[i];
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
//     ntg_scene_graph* graph = _scene->_graph;
//
//     struct _ntg_scene_node* node = ntg_scene_graph_get(graph, object);
//     const ntg_object_vec* children = ntg_object_get_children(object);
//     size_t child_count = children->_count;
//
//     ntg_object_size_map* sizes = ntg_object_size_map_new(
//             child_count, scene->__layout_arena);
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
        ntg_def_scene* scene)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    // struct _ntg_scene_node* node = ntg_scene_graph_get(graph, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->_count;

    ntg_object_xy_map* sizes = ntg_object_xy_map_new(
            child_count, scene->__layout_arena);

    ntg_object* it_child;
    struct ntg_scene_node_protect* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->_data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        ntg_object_xy_map_set(sizes, it_child, it_node->size);
    }

    return sizes;
}

static ntg_object_xy_map* get_object_pos(ntg_object* object,
        ntg_def_scene* scene)
{
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    // struct _ntg_scene_node* node = ntg_scene_graph_get(graph, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->_count;

    ntg_object_xy_map* positions = ntg_object_xy_map_new(
            child_count, scene->__layout_arena);

    ntg_object* it_child;
    struct ntg_scene_node_protect* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->_data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        ntg_object_xy_map_set(positions, it_child, it_node->position);
    }

    return positions;
}

static void measure1_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct ntg_scene_node_protect* node = ntg_scene_graph_get(graph, object);
    // const ntg_object_vec* children = ntg_object_get_children(object);
    // size_t child_count = children->_count;

    ntg_object_measure_map* measures = get_object_measure_map(object,
            scene, NTG_ORIENTATION_H);

    struct ntg_object_measure_ctx ctx = { .measures = measures };
    struct ntg_object_measure_out out = {0};

    struct ntg_object_measure result = ntg_object_measure(
            object, NTG_ORIENTATION_H,
            NTG_SIZE_MAX, ctx, &out, scene->__layout_arena);

    node->min_size.x = result.min_size;
    node->natural_size.x = result.natural_size;
    node->max_size.x = result.max_size;
    node->grow.x = result.grow;
}

static void constrain1_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct ntg_scene_node_protect* node = ntg_scene_graph_get(graph, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->_count;

    ntg_object_measure_map* measures = get_object_measure_map(object,
            scene, NTG_ORIENTATION_H);
    ntg_object_size_map* _sizes = ntg_object_size_map_new(child_count,
            scene->__layout_arena);

    struct ntg_object_constrain_ctx ctx = { .measures = measures };
    struct ntg_object_constrain_out out = { .sizes = _sizes };

    ntg_object_constrain(object,
            NTG_ORIENTATION_H,
            node->size.x, ctx,
            &out, scene->__layout_arena);

    size_t i;
    const ntg_object* it_child;
    struct ntg_scene_node_protect* it_node;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->_data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        it_node->size.x = ntg_object_size_map_get(out.sizes, it_child);
    }
}

static void measure2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct ntg_scene_node_protect* node = ntg_scene_graph_get(graph, object);
    // const ntg_object_vec* children = ntg_object_get_children(object);
    // size_t child_count = children->_count;

    ntg_object_measure_map* measures = get_object_measure_map(object,
            scene, NTG_ORIENTATION_V);

    struct ntg_object_measure_ctx ctx = { .measures = measures };
    struct ntg_object_measure_out out = {0};

    struct ntg_object_measure result = ntg_object_measure(
            object, NTG_ORIENTATION_V,
            node->size.x, ctx, &out, scene->__layout_arena);

    node->min_size.y = result.min_size;
    node->natural_size.y = result.natural_size;
    node->max_size.y = result.max_size;
    node->grow.y = result.grow;
}

static void constrain2_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct ntg_scene_node_protect* node = ntg_scene_graph_get(graph, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->_count;

    ntg_object_measure_map* measures = get_object_measure_map(object,
            scene, NTG_ORIENTATION_V);
    ntg_object_size_map* _sizes = ntg_object_size_map_new(child_count,
            scene->__layout_arena);

    struct ntg_object_constrain_ctx ctx = { .measures = measures };
    struct ntg_object_constrain_out out = { .sizes = _sizes };

    ntg_object_constrain(object,
            NTG_ORIENTATION_V,
            node->size.y, ctx,
            &out, scene->__layout_arena);

    size_t i;
    const ntg_object* it_child;
    struct ntg_scene_node_protect* it_node;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->_data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        it_node->size.y = ntg_object_size_map_get(out.sizes, it_child);
    }
}

static void arrange_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct ntg_scene_node_protect* node = ntg_scene_graph_get(graph, object);
    const ntg_object_vec* children = ntg_object_get_children(object);
    size_t child_count = children->_count;

    ntg_object_xy_map* sizes = get_object_size_map_xy(object, scene);
    ntg_object_xy_map* _positions = ntg_object_xy_map_new(child_count, scene->__layout_arena);

    size_t i;
    struct ntg_scene_node_protect* it_node;
    ntg_object* it_child;
    for(i = 0; i < child_count; i++)
    {
        it_child = children->_data[i];
        it_node = ntg_scene_graph_get(graph, it_child);

        ntg_object_xy_map_set(sizes, it_child, it_node->size);
    }

    struct ntg_object_arrange_ctx ctx = { .sizes = sizes };
    struct ntg_object_arrange_out out = { .pos = _positions };

    ntg_object_arrange(object, node->size, ctx, &out, scene->__layout_arena);

    for(i = 0; i < child_count; i++)
    {
        it_child = children->_data[i];
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
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct ntg_scene_node_protect* node = ntg_scene_graph_get(graph, object);
    ntg_object_drawing_set_size(node->drawing, node->size);

    ntg_object_xy_map* sizes = get_object_size_map_xy(object, scene);
    ntg_object_xy_map* positions = get_object_pos(object, scene);

    struct ntg_object_draw_ctx ctx = { .sizes = sizes, .pos = positions };
    struct ntg_object_draw_out out = { .drawing = node->drawing };

    ntg_object_draw(object, node->size, ctx, &out, scene->__layout_arena);
}

// static bool __process_key_fn(ntg_scene* _scene, struct nt_key_event key,
//         ntg_loop_ctx* loop_ctx)
// {
//     assert(_scene != NULL);
//
//     ntg_def_scene* scene = (ntg_def_scene*)_scene;
//
//     ntg_object* focused = _scene->_focused;
//
//     struct ntg_process_key_ctx ctx = {
//         .scene = _scene,
//         .loop_ctx = loop_ctx
//     };
//
//     bool consumed = false;
//     if(scene->__key_mode == NTG_DEF_SCENE_KEY_MODE_FOCUSED_FIRST)
//     {
//         consumed = scene->__process_key_fn(scene, key, loop_ctx);
//
//         if((!consumed) && (focused != NULL))
//             consumed = focused->_process_key_fn_(focused, key, ctx);
//     }
//     else
//     {
//         if(focused != NULL)
//             consumed = focused->_process_key_fn_(focused, key, ctx);
//
//         if(!consumed)
//             consumed = scene->__process_key_fn(scene, key, loop_ctx);
//     }
//
//     return consumed;
// }
