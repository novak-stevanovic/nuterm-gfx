#include <stdlib.h>
#include <assert.h>

#include "core/scene/ntg_def_scene.h"
#include "core/scene/_ntg_scene_graph.h"
#include "core/scene/ntg_drawable.h"
#include "core/scene/shared/ntg_drawable_kit.h"
#include "core/scene/shared/ntg_drawable_vec.h"
// #include "sarena.h"

static void __measure1_fn(ntg_drawable* drawable, void* _layout_data);
static void __constrain1_fn(ntg_drawable* drawable, void* _layout_data);
static void __measure2_fn(ntg_drawable* drawable, void* _layout_data);
static void __constrain2_fn(ntg_drawable* drawable, void* _layout_data);
static void __arrange_fn(ntg_drawable* drawable, void* _layout_data);
static void __draw_fn(ntg_drawable* drawable, void* _layout_data);

void __ntg_def_scene_init__(
        ntg_def_scene* scene,
        ntg_scene_process_key_fn process_key_fn)
{
    assert(scene != NULL);

    __ntg_scene_init__((ntg_scene*)scene,
            __ntg_def_scene_layout_fn,
            NULL,
            NULL,
            process_key_fn);
}

void __ntg_def_scene_deinit__(ntg_def_scene* scene)
{
    assert(scene != NULL);

    __ntg_scene_deinit__((ntg_scene*)scene);
}

struct ntg_layout_data
{
    // SArena* arena; 
    ntg_def_scene* scene;
};

void __ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size)
{
    assert(_scene != NULL);
    if(_scene->__root == NULL) return;

    ntg_def_scene* scene = (ntg_def_scene*)_scene;

    // SArena* arena = sarena_create(100000, NULL);
    // assert(arena != NULL);

    ntg_drawable* root = _scene->__root;

    struct ntg_layout_data data = {
        // .arena = arena,
        .scene = scene
    };

    struct _ntg_scene_node* root_data = ntg_scene_graph_get(_ntg_scene_get_graph(_scene), root);

    root_data->size = size;
    root_data->position = ntg_xy(0, 0);

    ntg_drawable_tree_perform(root, NTG_DRAWABLE_PERFORM_BOTTOM_UP,
            __measure1_fn, &data);

    ntg_drawable_tree_perform(root, NTG_DRAWABLE_PERFORM_TOP_DOWN,
            __constrain1_fn, &data);

    ntg_drawable_tree_perform(root, NTG_DRAWABLE_PERFORM_BOTTOM_UP,
            __measure2_fn, &data);

    ntg_drawable_tree_perform(root, NTG_DRAWABLE_PERFORM_TOP_DOWN,
            __constrain2_fn, &data);

    ntg_drawable_tree_perform(root, NTG_DRAWABLE_PERFORM_TOP_DOWN,
            __arrange_fn, &data);

    ntg_drawable_tree_perform(root, NTG_DRAWABLE_PERFORM_TOP_DOWN,
            __draw_fn, &data);

    // sarena_destroy(arena);
}

/* -------------------------------------------------------------------------- */

static void __measure1_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);
    ntg_drawable_vec_view children = drawable->_get_children_fn_(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_measure_context* context = ntg_measure_context_new();

    ntg_drawable* it_drawable;
    struct ntg_measure_data it_data;
    struct _ntg_scene_node* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_data = (struct ntg_measure_data) {
            .min_size = it_node->min_size.x,
            .natural_size = it_node->natural_size.x,
            .max_size = it_node->max_size.x,
            .grow = it_node->grow.x
        };

        ntg_measure_context_set(context, it_drawable, it_data);
    }

    struct ntg_measure_output result = drawable->_measure_fn(
            drawable, NTG_ORIENTATION_HORIZONTAL,
            NTG_SIZE_MAX, context);

    node->min_size.x = result.min_size;
    node->natural_size.x = result.natural_size;
    node->max_size.x = result.max_size;
    node->grow.x = result.grow;

    ntg_measure_context_destroy(context);
}

static void __constrain1_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);
    ntg_drawable_vec_view children = drawable->_get_children_fn_(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_constrain_context* context = ntg_constrain_context_new();
    ntg_constrain_output* output = ntg_constrain_output_new();

    ntg_drawable* it_drawable;
    struct ntg_constrain_data it_data;
    struct _ntg_scene_node* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_data = (struct ntg_constrain_data) {
            .min_size = it_node->min_size.x,
            .natural_size = it_node->natural_size.x,
            .max_size = it_node->max_size.x,
            .grow = it_node->grow.x
        };

        ntg_constrain_context_set(context, it_drawable, it_data);
    }

    struct ntg_measure_output measure_output = {
        .min_size = node->min_size.x,
        .natural_size = node->natural_size.x,
        .max_size = node->max_size.x,
        .grow = node->grow.x
    };

    drawable->_constrain_fn(drawable,
            NTG_ORIENTATION_HORIZONTAL,
            node->size.x, measure_output,
            context, output);

    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_node->size.x = ntg_constrain_output_get(output, it_drawable).size;
    }

    ntg_constrain_context_destroy(context);
    ntg_constrain_output_destroy(output);
}

static void __measure2_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);

    ntg_drawable_vec_view children = drawable->_get_children_fn_(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_measure_context* context = ntg_measure_context_new();

    ntg_drawable* it_drawable;
    struct ntg_measure_data it_data;
    struct _ntg_scene_node* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_data = (struct ntg_measure_data) {
            .min_size = it_node->min_size.y,
            .natural_size = it_node->natural_size.y,
            .max_size = it_node->max_size.y,
            .grow = it_node->grow.y
        };

        ntg_measure_context_set(context, it_drawable, it_data);
    }

    struct ntg_measure_output result = drawable->_measure_fn(
            drawable, NTG_ORIENTATION_VERTICAL,
            node->size.x, context);

    node->min_size.y = result.min_size;
    node->natural_size.y = result.natural_size;
    node->max_size.y = result.max_size;
    node->grow.y = result.grow;

    ntg_measure_context_destroy(context);

}

static void __constrain2_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);

    ntg_drawable_vec_view children = drawable->_get_children_fn_(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_constrain_context* context = ntg_constrain_context_new();
    ntg_constrain_output* output = ntg_constrain_output_new();

    ntg_drawable* it_drawable;
    struct ntg_constrain_data it_data;
    struct _ntg_scene_node* it_node;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_data = (struct ntg_constrain_data) {
            .min_size = it_node->min_size.y,
            .natural_size = it_node->natural_size.y,
            .max_size = it_node->max_size.y,
            .grow = it_node->grow.y
        };

        ntg_constrain_context_set(context, it_drawable, it_data);
    }

    struct ntg_measure_output measure_output = {
        .min_size = node->min_size.y,
        .natural_size = node->natural_size.y,
        .max_size = node->max_size.y,
        .grow = node->grow.y
    };

    drawable->_constrain_fn(drawable,
            NTG_ORIENTATION_VERTICAL,
            node->size.y, measure_output,
            context, output);

    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_node->size.y = ntg_constrain_output_get(output, it_drawable).size;
    }

    ntg_constrain_context_destroy(context);
    ntg_constrain_output_destroy(output);
}

static void __arrange_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);

    ntg_drawable_vec_view children = drawable->_get_children_fn_(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_arrange_context* context = ntg_arrange_context_new();
    ntg_arrange_output* output = ntg_arrange_output_new();

    size_t i;
    struct ntg_arrange_data it_data;
    struct _ntg_scene_node* it_node;
    ntg_drawable* it_drawable;
    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_data = (struct ntg_arrange_data) {
            .size = it_node->size
        };

        ntg_arrange_context_set(context, it_drawable, it_data);
    }

    drawable->_arrange_fn(drawable, node->size, context, output);

    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_node->position = ntg_xy_add(node->position,
                ntg_arrange_output_get(output, it_drawable).pos);
    }

    ntg_arrange_context_destroy(context);
    ntg_arrange_output_destroy(output);
}

static void __draw_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _ntg_scene_get_graph(_scene);

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);
    ntg_drawing_set_size(node->drawing, node->size);
    if(drawable->_draw_fn)
        drawable->_draw_fn(drawable, node->size, node->drawing);
}
