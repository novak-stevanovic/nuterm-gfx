#include <stdlib.h>
#include <assert.h>

#include "core/scene/ntg_def_scene.h"
#include "core/scene/_ntg_scene_graph.h"
#include "core/scene/ntg_drawable.h"
#include "core/scene/shared/ntg_drawable_kit.h"
#include "core/scene/shared/ntg_drawable_vec.h"
#include "shared/sarena.h"

static void __measure1_fn(ntg_drawable* drawable, void* _layout_data);
static void __constrain1_fn(ntg_drawable* drawable, void* _layout_data);
static void __measure2_fn(ntg_drawable* drawable, void* _layout_data);
static void __constrain2_fn(ntg_drawable* drawable, void* _layout_data);
static void __arrange_fn(ntg_drawable* drawable, void* _layout_data);
static void __draw_fn(ntg_drawable* drawable, void* _layout_data);
static bool __process_key_fn(ntg_scene* _scene, struct nt_key_event key,
        ntg_loop_ctx* loop_ctx);

void __ntg_def_scene_init__(
        ntg_def_scene* scene,
        ntg_def_scene_process_key_fn process_key_fn,
        void* data)
{
    assert(scene != NULL);

    __ntg_scene_init__((ntg_scene*)scene,
            __ntg_def_scene_layout_fn,
            NULL,
            NULL,
            __process_key_fn,
            data);

    scene->__process_key_mode = NTG_DEF_SCENE_PROCESS_KEY_FOCUSD_FIRST;
    scene->__process_key_fn = process_key_fn;

    sa_err _err;
    scene->_layout_arena = sarena_create(100000, &_err);
    assert(scene->_layout_arena != NULL);
}

void __ntg_def_scene_deinit__(ntg_def_scene* scene)
{
    assert(scene != NULL);

    __ntg_scene_deinit__((ntg_scene*)scene);

    scene->__process_key_mode = NTG_DEF_SCENE_PROCESS_KEY_FOCUSD_FIRST;
    scene->__process_key_fn = NULL;

    sarena_destroy(scene->_layout_arena);
    scene->_layout_arena = NULL;
}

void ntg_def_scene_set_process_key_mode(ntg_def_scene* scene,
        ntg_def_scene_process_key_mode mode)
{
    assert(scene != NULL);

    scene->__process_key_mode = mode;
}

ntg_def_scene_process_key_mode ntg_def_scene_get_process_key_mode(
        const ntg_def_scene* scene)
{
    assert(scene != NULL);

    return scene->__process_key_mode;
}

struct ntg_layout_data
{
    ntg_def_scene* scene;
};

void __ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size)
{
    assert(_scene != NULL);

    ntg_drawable* root = ntg_scene_get_root(_scene);
    if(_scene->_root == NULL) return;

    ntg_def_scene* scene = (ntg_def_scene*)_scene;

    struct ntg_layout_data data = {
        .scene = scene
    };

    struct _ntg_scene_node* root_data = ntg_scene_graph_get(_scene->_graph, root);

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

    sarena_rewind(scene->_layout_arena);
}

/* -------------------------------------------------------------------------- */

static void __measure1_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);
    ntg_drawable_vec_view children = drawable->_get_children_fn__(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_measure_ctx* ctx = ntg_measure_ctx_new(child_count, scene->_layout_arena);

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

        ntg_measure_ctx_set(ctx, it_drawable, it_data);
    }

    struct ntg_measure_out result = drawable->_measure_fn_(
            drawable, NTG_ORIENTATION_H,
            NTG_SIZE_MAX, ctx, scene->_layout_arena);

    node->min_size.x = result.min_size;
    node->natural_size.x = result.natural_size;
    node->max_size.x = result.max_size;
    node->grow.x = result.grow;
}

static void __constrain1_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);
    ntg_drawable_vec_view children = drawable->_get_children_fn__(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_constrain_ctx* constrain_ctx = ntg_constrain_ctx_new(child_count, scene->_layout_arena);
    ntg_measure_ctx* measure_ctx = ntg_measure_ctx_new(child_count, scene->_layout_arena);
    ntg_constrain_out* out = ntg_constrain_out_new(child_count, scene->_layout_arena);

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

        ntg_measure_ctx_set(measure_ctx, it_drawable, it_data);
    }

    drawable->_constrain_fn_(drawable,
            NTG_ORIENTATION_H,
            node->size.x, constrain_ctx,
            measure_ctx, out, scene->_layout_arena);

    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_node->size.x = ntg_constrain_out_get(out, it_drawable).size;
    }
}

static void __measure2_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);

    ntg_drawable_vec_view children = drawable->_get_children_fn__(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_measure_ctx* ctx = ntg_measure_ctx_new(child_count, scene->_layout_arena);

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

        ntg_measure_ctx_set(ctx, it_drawable, it_data);
    }

    struct ntg_measure_out result = drawable->_measure_fn_(
            drawable, NTG_ORIENTATION_V,
            node->size.x, ctx, scene->_layout_arena);

    node->min_size.y = result.min_size;
    node->natural_size.y = result.natural_size;
    node->max_size.y = result.max_size;
    node->grow.y = result.grow;
}

static void __constrain2_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);

    ntg_drawable_vec_view children = drawable->_get_children_fn__(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_constrain_ctx* constrain_ctx = ntg_constrain_ctx_new(child_count, scene->_layout_arena);
    ntg_measure_ctx* measure_ctx = ntg_measure_ctx_new(child_count, scene->_layout_arena);
    ntg_constrain_out* out = ntg_constrain_out_new(child_count, scene->_layout_arena);

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

        ntg_measure_ctx_set(measure_ctx, it_drawable, it_data);
    }

    drawable->_constrain_fn_(drawable,
            NTG_ORIENTATION_V,
            node->size.y, constrain_ctx,
            measure_ctx, out, scene->_layout_arena);

    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_node->size.y = ntg_constrain_out_get(out, it_drawable).size;
    }
}

static void __arrange_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);

    ntg_drawable_vec_view children = drawable->_get_children_fn__(drawable);
    size_t child_count = ntg_drawable_vec_view_count(&children);

    ntg_arrange_ctx* ctx = ntg_arrange_ctx_new(child_count, scene->_layout_arena);
    ntg_arrange_out* out = ntg_arrange_out_new(child_count, scene->_layout_arena);

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

        ntg_arrange_ctx_set(ctx, it_drawable, it_data);
    }

    drawable->_arrange_fn_(drawable, node->size, ctx, out, scene->_layout_arena);

    for(i = 0; i < child_count; i++)
    {
        it_drawable = ntg_drawable_vec_view_at(&children, i);
        it_node = ntg_scene_graph_get(graph, it_drawable);

        it_node->position = ntg_xy_add(node->position,
                ntg_arrange_out_get(out, it_drawable).pos);
    }
}

static void __draw_fn(ntg_drawable* drawable, void* _layout_data)
{
    struct ntg_layout_data* data = (struct ntg_layout_data*)_layout_data;
    ntg_def_scene* scene = data->scene; 
    ntg_scene* _scene = (ntg_scene*)scene;
    ntg_scene_graph* graph = _scene->_graph;

    struct _ntg_scene_node* node = ntg_scene_graph_get(graph, drawable);
    ntg_drawing_set_size(node->drawing, node->size);
    if(drawable->_draw_fn_)
        drawable->_draw_fn_(drawable, node->size, node->drawing, scene->_layout_arena);
}

static bool __process_key_fn(ntg_scene* _scene, struct nt_key_event key,
        ntg_loop_ctx* loop_ctx)
{
    assert(_scene != NULL);

    ntg_def_scene* scene = (ntg_def_scene*)_scene;

    ntg_drawable* focused = _scene->_focused;

    struct ntg_process_key_ctx ctx = {
        .scene = _scene,
        .loop_ctx = loop_ctx
    };

    bool consumed = false;
    if(scene->__process_key_mode == NTG_DEF_SCENE_PROCESS_KEY_SCENE_FIRST)
    {
        consumed = scene->__process_key_fn(scene, key, loop_ctx);

        if((!consumed) && (focused != NULL))
            consumed = focused->_process_key_fn_(focused, key, ctx);
    }
    else
    {
        if(focused != NULL)
            consumed = focused->_process_key_fn_(focused, key, ctx);

        if(!consumed)
            consumed = scene->__process_key_fn(scene, key, loop_ctx);
    }

    return consumed;
}
