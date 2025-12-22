#include "core/scene/ntg_scene.h"

#include <assert.h>
#include <stdlib.h>

#include "base/event/ntg_event.h"
#include "core/object/ntg_object.h"
#include "core/object/shared/ntg_object_vec.h"
#include "shared/ntg_log.h"

static void update_focused(ntg_scene* scene);
static void scan_scene(ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_scene* scene);
static bool process_key_fn_def(ntg_scene* scene,
        struct nt_key_event key, ntg_loop_ctx* loop_ctx);

void _ntg_scene_init_(
        ntg_scene* scene,
        ntg_scene_layout_fn layout_fn,
        ntg_scene_on_register_fn on_register_fn,
        ntg_scene_on_unregister_fn on_unregister_fn,
        ntg_scene_process_key_fn process_key_fn,
        ntg_scene_deinit_fn deinit_fn,
        void* data)
{
    assert(scene != NULL);
    assert(layout_fn != NULL);

    __init_default_values(scene);

    scene->_root = NULL;
    scene->_focused = NULL;

    scene->__layout_fn = layout_fn;
    scene->__on_register_fn = on_register_fn;
    scene->__on_unregister_fn = on_unregister_fn;
    scene->__process_key_fn = (process_key_fn != NULL) ?
        process_key_fn : process_key_fn_def;
    scene->__deinit_fn = (deinit_fn != NULL) ? deinit_fn : _ntg_scene_deinit_;
    scene->_data = data;

    scene->_delegate = ntg_event_dlgt_new();
    scene->_graph = ntg_scene_graph_new();
}

static void __init_default_values(ntg_scene* scene)
{
    (*scene) = (ntg_scene) {0};
}

void _ntg_scene_deinit_(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_event_dlgt_destroy(scene->_delegate);
    ntg_scene_graph_destroy(scene->_graph);

    __init_default_values(scene);
}

void _ntg_scene_vdeinit_(ntg_scene* scene)
{
    assert(scene != NULL);

    scene->__deinit_fn(scene);
}

static bool process_key_fn_def(ntg_scene* scene,
        struct nt_key_event key, ntg_loop_ctx* loop_ctx)
{
    assert(scene != NULL);

    struct ntg_object_key_ctx ctx = {
        .scene = scene,
        .loop_ctx = loop_ctx
    };

    return ntg_object_feed_key(scene->_focused, key, ctx);
}

ntg_object* ntg_scene_get_focused(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->_focused;
}

void ntg_scene_focus(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);

    scene->__pending_focused = object;
    scene->__pending_focused_flag = true;
}

ntg_object* ntg_scene_get_root(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->_root;
}

void ntg_scene_set_root(ntg_scene* scene, ntg_object* root)
{
    assert(scene != NULL);

    scene->_root = root;
}

struct ntg_scene_node ntg_scene_get_node(const ntg_scene* scene,
        const ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    struct ntg_scene_node_protect* _node = ntg_scene_graph_get(scene->_graph, object);
    assert(_node != NULL);

    return (struct ntg_scene_node) {
        .min_size = _node->min_size,
        .natural_size = _node->natural_size,
        .max_size = _node->max_size,
        .size = _node->size,
        .grow = _node->grow,
        .position = _node->position,
        .drawing = _node->drawing
    };
}

/* ------------------------------------------------------ */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene != NULL);

    ntg_log_log("Layout begin");

    scene->_size = size;

    scan_scene(scene);

    update_focused(scene);

    ntg_log_log("Layout fn");

    scene->__layout_fn(scene, size);
}

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->_size;
}

/* ------------------------------------------------------ */

bool ntg_scene_feed_key_event(
        ntg_scene* scene,
        struct nt_key_event key,
        ntg_loop_ctx* loop_ctx)
{
    assert(scene != NULL);

    return scene->__process_key_fn(scene, key, loop_ctx);
}

ntg_listenable* ntg_scene_get_listenable(ntg_scene* scene)
{
    assert(scene != NULL);

    return ntg_event_dlgt_listenable(scene->_delegate);
}

/* ------------------------------------------------------ */

static void add_to_registered_fn(ntg_object* object, void* _registered)
{
    ntg_object_vec* registered = (ntg_object_vec*)_registered;

    ntg_object_vec_append(registered, object);
}

static void update_focused(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_object* old = scene->_focused;
    ntg_object* new = scene->__pending_focused;

    if(scene->__pending_focused_flag)
    {
        if(old != NULL)
        {
            // struct ntg_object_unfocus_ctx ctx = {
            //     .new = new,
            //     .scene = scene
            // };
            //
            // ntg_object_on_unfocus(old, ctx);
        }

        if(new != NULL)
        {
            struct ntg_scene_node_protect* node = ntg_scene_graph_get(scene->_graph, new);
            assert(node != NULL);

            scene->_focused = new;

            // struct ntg_object_focus_ctx focus_ctx = {
            //     .old = old,
            //     .scene = scene
            // };

            // ntg_object_on_focus(new, focus_ctx);

            scene->__pending_focused = NULL;
        }

        scene->__pending_focused_flag = false;
    }

}

static void scan_scene(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_object_vec current;
    _ntg_object_vec_init_(&current);
    if(scene->_root != NULL)
    {
        ntg_object_tree_perform(
                scene->_root,
                NTG_OBJECT_PERFORM_TOP_DOWN,
                add_to_registered_fn,
                &current);
    }
    
    ntg_const_object_vec alr_registered; 
    _ntg_const_object_vec_init_(&alr_registered);
    ntg_scene_graph_get_keys(scene->_graph, &alr_registered);

    size_t i;
    const ntg_object* it_object;
    for(i = 0; i < alr_registered._count; i++)
    {
        it_object = alr_registered._data[i];

        // Drawable removed from the scene
        if(!(ntg_object_vec_contains(&current, it_object)))
        {
            struct ntg_scene_node_protect* node = ntg_scene_graph_get(
                    scene->_graph, it_object);
            assert(node != NULL);

            ntg_object_layout_deinit(it_object, node->object_layout_data);
            node->object_layout_data = NULL;

            ntg_scene_graph_remove(scene->_graph, it_object);

            if(scene->__on_unregister_fn != NULL)
                scene->__on_unregister_fn(scene, it_object);
        }
    }

    for(i = 0; i < current._count; i++)
    {
        it_object = current._data[i];

        // Drawable added to the scene
        if(!(ntg_const_object_vec_contains(&alr_registered, it_object)))
        {
            ntg_scene_graph_add(scene->_graph, it_object);

            struct ntg_scene_node_protect* node = ntg_scene_graph_get(
                    scene->_graph, it_object);
            assert(node != NULL);

            node->object_layout_data = ntg_object_layout_init(it_object);

            if(scene->__on_register_fn != NULL)
                scene->__on_register_fn(scene, it_object);
        }
    }

    _ntg_object_vec_deinit_(&current);
    _ntg_const_object_vec_deinit_(&alr_registered);
}
