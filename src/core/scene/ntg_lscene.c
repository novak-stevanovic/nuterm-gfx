#include <stdlib.h>
#include <assert.h>

#include "core/scene/ntg_lscene.h"
#include "core/scene/_ntg_lscene_graph.h"
#include "core/scene/ntg_drawable.h"
#include "core/scene/shared/ntg_measure_context.h"
#include "core/scene/shared/ntg_measure_output.h"
#include "core/scene/shared/ntg_constrain_context.h"
#include "core/scene/shared/ntg_constrain_output.h"
#include "core/scene/shared/ntg_arrange_context.h"
#include "core/scene/shared/ntg_arrange_output.h"
#include "core/scene/shared/ntg_drawable_vec.h"
#include "core/scene/shared/ntg_drawing.h"
#include "sarena.h"

static void __ntg_lscene_layout_fn(ntg_scene* _scene, struct ntg_xy size);
static void __ntg_lscene_on_register_fn(ntg_scene* _scene, const ntg_drawable* drawable);
static void __ntg_lscene_on_unregister_fn(ntg_scene* _scene, const ntg_drawable* drawable);

void __ntg_lscene_init__(
        ntg_lscene* scene,
        ntg_drawable* root,
        ntg_scene_process_key_fn process_key_fn)
{
    assert(scene != NULL);

    __ntg_scene_init__((ntg_scene*)scene,
            root,
            __ntg_lscene_layout_fn,
            __ntg_lscene_on_register_fn,
            __ntg_lscene_on_unregister_fn,
            process_key_fn);

    scene->__graph = ntg_lscene_graph_new();
}

void __ntg_lscene_deinit__(ntg_lscene* scene)
{
    assert(scene != NULL);

    __ntg_scene_deinit__((ntg_scene*)scene);

    ntg_lscene_graph_destroy(scene->__graph);
    scene->__graph = NULL;
}

struct __layout_data
{
    SArena* arena; 
    ntg_lscene_graph* graph;
};

static void __ntg_lscene_layout_fn(ntg_scene* _scene, struct ntg_xy size)
{
    assert(_scene != NULL);

    ntg_lscene* scene = (ntg_lscene*)_scene;

    SArena* arena = sarena_create(100000, NULL);
    assert(arena != NULL);

    ntg_drawable* root = _scene->__root;

    struct __layout_data data = {
        .arena = arena,
        .graph = scene->__graph
    };

    struct ntg_lscene_data* root_data = ntg_lscene_graph_get(scene->__graph, root);

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
            __arrange_children_fn, &data);

    ntg_drawable_tree_perform(root, NTG_DRAWABLE_PERFORM_TOP_DOWN,
            __draw_fn, &data);

    sarena_destroy(arena);
}

static void __ntg_lscene_on_register_fn(ntg_scene* _scene, const ntg_drawable* drawable)
{
    assert(_scene != NULL);
    assert(drawable != NULL);

    ntg_lscene* scene = (ntg_lscene*)_scene;
    ntg_lscene_graph_remove(scene->__graph, drawable);
}

static void __ntg_lscene_on_unregister_fn(ntg_scene* _scene, const ntg_drawable* drawable)
{
    assert(_scene != NULL);
    assert(drawable != NULL);

    ntg_lscene* scene = (ntg_lscene*)_scene;
    ntg_lscene_graph_remove(scene->__graph, drawable);
}
static void __measure1_fn(ntg_drawable* drawable, void* _);
static void __constrain1_fn(ntg_drawable* drawable, void* _);
static void __measure2_fn(ntg_drawable* drawable, void* _);
static void __constrain2_fn(ntg_drawable* drawable, void* _);
static void __arrange_children_fn(ntg_drawable* drawable, void* _);
static void __draw_fn(ntg_drawable* drawable, void* _);

/* -------------------------------------------------------------------------- */

static void __measure1_fn(ntg_drawable* drawable, void* _layout_data)
{
    ntg_lscene_graph* graph = ((struct __layout_data*)_layout_data)->graph;
    struct ntg_lscene_data* node_data = ntg_lscene_graph_get(graph, drawable);
    ntg_drawable_vec_view children = drawable->_get_children_fn_(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_measure_context* context = ntg_measure_context_new();

    ntg_drawable* it_drawable;
    struct ntg_measure_data it_measure_data;
    struct ntg_lscene_data* it_node_data;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node_data = ntg_lscene_graph_get(graph, it_drawable);

        it_measure_data = (struct ntg_measure_data) {
            .min_size = it_node_data->min_size.x,
            .natural_size = it_node_data->natural_size.x,
            .max_size = it_node_data->max_size.x
        };

        ntg_measure_context_set(context, it_drawable, it_measure_data);
    }

    struct ntg_measure_output result = drawable->_measure_fn(
            drawable, NTG_ORIENTATION_HORIZONTAL,
            NTG_SIZE_MAX, context);

    node_data->min_size.x = result.min_size;
    node_data->natural_size.x = result.natural_size;
    node_data->max_size.x = result.max_size;

    ntg_measure_context_destroy(context);
}

static void __constrain1_fn(ntg_drawable* drawable, void* _layout_data)
{
    ntg_lscene_graph* graph = ((struct __layout_data*)_layout_data)->graph;
    struct ntg_lscene_data* node_data = ntg_lscene_graph_get(graph, drawable);
    ntg_drawable_vec_view children = drawable->_get_children_fn_(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_drawable* it_drawable;
    struct ntg_measure_data it_constrain_data;
    struct ntg_lscene_data* it_node_data;
    size_t i;
    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node_data = ntg_lscene_graph_get(graph, it_drawable);

        it_constrain_data = (struct ntg_constrain_data) {
            .min_size = it_node_data->min_size.x,
            .natural_size = ntg_drawable_get_natural_size(it_drawable).x,
            .max_size = ntg_drawable_get_max_size(it_drawable).x,
            .grow = ntg_drawable_get_grow(it_drawable).x
        };

        ntg_constrain_context_set(context, it_drawable, it_data);
    }

    ntg_constrain_context_set_min_size(context, drawable->__min_size.x);
    ntg_constrain_context_set_natural_size(context, drawable->__natural_size.x);
    ntg_constrain_context_set_max_size(context, drawable->__max_size.x);

    _ntg_drawable_constrain(drawable, NTG_ORIENTATION_HORIZONTAL,
            drawable->__size.x, context, output);

    for(i = 0; i < children->_count; i++)
    {
        it_drawable = children->_data[i];

        it_drawable->__size.x = ntg_constrain_output_get(output, it_drawable).size;
    }

    ntg_constrain_context_destroy(context);
    ntg_constrain_output_destroy(output);
}

static void __measure2_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct __layout_data data = *(struct __layout_data*)___layout_data;
    ntg_measure_context* context = ntg_measure_context_new(drawable, data.arena);
    ntg_drawable_vec* children = _ntg_drawable_get_children(drawable);

    size_t i;
    struct ntg_measure_data it_data;
    ntg_drawable* it_drawable;
    for(i = 0; i < children->_count; i++)
    {
        it_drawable = children->_data[i];

        it_data = (struct ntg_measure_data) {
            .min_size = ntg_drawable_get_min_size(it_drawable).y,
            .natural_size = ntg_drawable_get_natural_size(it_drawable).y,
            .max_size = ntg_drawable_get_max_size(it_drawable).y
        };

        ntg_measure_context_set(context, it_drawable, it_data);
    }

    struct ntg_measure_result result = _ntg_drawable_measure(
            drawable, NTG_ORIENTATION_VERTICAL,
            drawable->__size.x, context);

    drawable->__min_size.y = result.min_size;
    drawable->__natural_size.y = result.natural_size;
    drawable->__max_size.y = result.max_size;

    ntg_measure_context_destroy(context);
}

static void __constrain2_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct __layout_data data = *(struct __layout_data*)_layout_data;
    ntg_constrain_context* context = ntg_constrain_context_new(drawable, data.arena);
    ntg_constrain_output* output = ntg_constrain_output_new(drawable, data.arena);
    ntg_drawable_vec* children = _ntg_drawable_get_children(drawable);

    size_t i;
    struct ntg_constrain_data it_data;
    ntg_drawable* it_drawable;
    for(i = 0; i < children->_count; i++)
    {
        it_drawable = children->_data[i];

        it_data = (struct ntg_constrain_data) {
            .min_size = ntg_drawable_get_min_size(it_drawable).y,
            .natural_size = ntg_drawable_get_natural_size(it_drawable).y,
            .max_size = ntg_drawable_get_max_size(it_drawable).y,
            .grow = ntg_drawable_get_grow(it_drawable).y
        };

        ntg_constrain_context_set(context, it_drawable, it_data);
    }

    ntg_constrain_context_set_min_size(context, drawable->__min_size.y);
    ntg_constrain_context_set_natural_size(context, drawable->__natural_size.y);
    ntg_constrain_context_set_max_size(context, drawable->__max_size.y);

    _ntg_drawable_constrain(drawable, NTG_ORIENTATION_VERTICAL, drawable->__size.y,
            context, output);

    for(i = 0; i < children->_count; i++)
    {
        it_drawable = children->_data[i];

        it_drawable->__size.y = ntg_constrain_output_get(output, it_drawable).size;
    }

    ntg_constrain_context_destroy(context);
    ntg_constrain_output_destroy(output);
}

static void __arrange_children_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct __layout_data data = *(struct __layout_data*)_layout_data;
    ntg_arrange_context* context = ntg_arrange_context_new(drawable, data.arena);
    ntg_arrange_output* output = ntg_arrange_output_new(drawable, data.arena);
    ntg_drawable_vec* children = _ntg_drawable_get_children(drawable);

    size_t i;
    struct ntg_arrange_data it_data;
    ntg_drawable* it_drawable;
    for(i = 0; i < children->_count; i++)
    {
        it_drawable = children->_data[i];

        it_data = (struct ntg_arrange_data) {
            .size = ntg_drawable_get_size(it_drawable)
        };

        ntg_arrange_context_set(context, it_drawable, it_data);
    }

    _ntg_drawable_arrange_children(drawable, drawable->__size, context, output);

    for(i = 0; i < children->_count; i++)
    {
        it_drawable = children->_data[i];

        it_drawable->__position = ntg_arrange_output_get(output, it_drawable).pos;
    }

    ntg_arrange_context_destroy(context);
    ntg_arrange_output_destroy(output);
}

static void __draw_fn(ntg_drawable* drawable, void* _layout_data)
{
}
