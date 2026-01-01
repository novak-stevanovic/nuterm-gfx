#include <stdlib.h>
#include <assert.h>

#include "core/scene/ntg_scene.h"
#include "core/loop/ntg_loop.h"
#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/scene/focuser/ntg_focuser.h"
#include "core/stage/ntg_stage.h"
#include "core/scene/ntg_scene_graph.h"

static void on_object_register(ntg_scene* scene, ntg_object* object);
static void on_object_unregister(ntg_scene* scene, ntg_object* object);
static void on_object_register_fn(ntg_object* object, void* _scene);
static void on_object_unregister_fn(ntg_object* object, void* _scene);
static void object_observe_fn(ntg_entity* scene, struct ntg_event event);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene != NULL);

    if(scene->_root != NULL)
        scene->__layout_fn(scene, size);
}

void ntg_scene_set_root(ntg_scene* scene, ntg_object* root)
{
    assert(scene != NULL);
    if(scene->_root == root) return;
    if(root != NULL)
        assert(ntg_object_get_parent(root, false) == NULL);

    ntg_object* old_root = scene->_root;

    if(old_root != NULL)
    {
        ntg_object* old_root_gr = ntg_object_get_group_root_(old_root);
        ntg_object_tree_perform(old_root_gr, NTG_OBJECT_PERFORM_TOP_DOWN,
                on_object_unregister_fn, scene);
    }

    if(root != NULL)
    {
        ntg_object* root_gr = ntg_object_get_group_root_(root);

        ntg_object_tree_perform(root_gr, NTG_OBJECT_PERFORM_TOP_DOWN,
                on_object_register_fn, scene);
    }

    scene->_root = root;
}

struct ntg_scene_node ntg_scene_get_node(
        const ntg_scene* scene,
        const ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    struct ntg_scene_node_protected* _node = ntg_scene_graph_get(
            scene->__graph, object);

    if(_node == NULL) return (struct ntg_scene_node) {0};

    return (struct ntg_scene_node) {
        .exists = true,
        .min_size = _node->min_size,
        .natural_size = _node->natural_size,
        .max_size = _node->max_size,
        .size = _node->size,
        .grow = _node->grow,
        .position = _node->position,
        .drawing = _node->drawing
    };
}

bool ntg_scene_feed_event(
        ntg_scene* scene,
        struct ntg_loop_event event,
        ntg_loop_ctx* loop_ctx)
{
    assert(scene != NULL);

    bool consumed = false;
    if(scene->__event_mode == NTG_SCENE_EVENT_PROCESS_FIRST)
    {
        if(scene->__event_fn != NULL)
            consumed = scene->__event_fn(scene, event, loop_ctx);

        if(!consumed && scene->_focuser != NULL)
            ntg_focuser_dispatch_event(scene->_focuser, event, loop_ctx);
    }
    else // scene->__event_mode = NTG_SCENE_EVENT_DISPATCH_FIRST
    {
        if(scene->_focuser != NULL)
            ntg_focuser_dispatch_event(scene->_focuser, event, loop_ctx);

        if(!consumed && (scene->__event_fn != NULL))
            consumed = scene->__event_fn(scene, event, loop_ctx);
    }

    return consumed;
}

void ntg_scene_set_event_fn(ntg_scene* scene, ntg_scene_event_fn fn)
{
    assert(scene != NULL);

    scene->__event_fn = fn;
}

void ntg_scene_set_event_mode(ntg_scene* scene, ntg_scene_event_mode mode)
{
    assert(scene != NULL);

    scene->__event_mode = mode;
}

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_scene* scene);

void _ntg_scene_init_(ntg_scene* scene,
        ntg_focuser* focuser,
        ntg_scene_layout_fn layout_fn)
{
    assert(scene != NULL);
    assert(layout_fn != NULL);

    __init_default_values(scene);

    scene->_focuser = focuser;
    scene->__layout_fn = layout_fn;

    scene->__graph = ntg_scene_graph_new();
}

static void __init_default_values(ntg_scene* scene)
{
    scene->_stage = NULL;

    scene->_root = NULL;
    scene->__graph = NULL;
    scene->__layout_fn = NULL;

    scene->_focuser = NULL;

    scene->__event_fn = NULL;
    scene->__event_mode = NTG_SCENE_EVENT_DISPATCH_FIRST;

    scene->data = NULL;
}

void _ntg_scene_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_scene* scene = (ntg_scene*)entity;
    ntg_scene_graph_destroy(scene->__graph);
    __init_default_values(scene);
}

ntg_scene_graph* _ntg_scene_get_graph(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__graph;
}

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage)
{
    assert(scene != NULL);

    scene->_stage = stage;
}

static void on_object_register(ntg_scene* scene, ntg_object* object)
{
    ntg_entity_observe((ntg_entity*)scene, (ntg_entity*)object,
            object_observe_fn);

    struct ntg_scene_node_protected* node = ntg_scene_graph_get(
            scene->__graph, object);
    assert(node == NULL);

    struct ntg_scene_node_protected* node_new = ntg_scene_graph_add(scene->__graph, object);
    assert(node_new != NULL);

    node_new->object_layout_data = ntg_object_layout_init(object);

    _ntg_object_set_scene(object, scene);

    struct ntg_event_scene_objadd_data data = { .object = object };
    ntg_entity_raise_event((ntg_entity*)scene, NULL, NTG_EVENT_SCENE_OBJADD, &data);
}

static void on_object_unregister(ntg_scene* scene, ntg_object* object)
{
    ntg_entity_stop_observing((ntg_entity*)scene, (ntg_entity*)object,
            object_observe_fn);

    struct ntg_scene_node_protected* node = ntg_scene_graph_get(scene->__graph, object);
    assert(node != NULL);

    node->object_layout_data = ntg_object_layout_init(object);

    ntg_scene_graph_remove(scene->__graph, object);

    _ntg_object_set_scene(object, NULL);

    struct ntg_event_scene_objrm_data data = { .object = object };
    ntg_entity_raise_event((ntg_entity*)scene, NULL, NTG_EVENT_SCENE_OBJRM, &data);
}

static void on_object_register_fn(ntg_object* object, void* _scene)
{
    on_object_register((ntg_scene*)_scene, object);
}

static void on_object_unregister_fn(ntg_object* object, void* _scene)
{
    on_object_unregister((ntg_scene*)_scene, object);
}

static void object_observe_fn(ntg_entity* scene, struct ntg_event event)
{
    if(event.type == NTG_EVENT_OBJECT_CHLDADD_DCR)
    {
        struct ntg_event_object_chldadd_data* data = event.data;
        ntg_object_tree_perform(data->child, NTG_OBJECT_PERFORM_TOP_DOWN,
                on_object_register_fn, scene);
    }
    else if(event.type == NTG_EVENT_OBJECT_CHLDRM_DCR)
    {
        struct ntg_event_object_chldrm_data* data = event.data;
        ntg_object_tree_perform(data->child, NTG_OBJECT_PERFORM_TOP_DOWN,
                on_object_unregister_fn, scene);
    }
}
