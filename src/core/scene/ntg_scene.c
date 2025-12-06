#include "core/scene/ntg_scene.h"

#include <assert.h>
#include <stdlib.h>

#include "base/event/ntg_event.h"
#include "core/scene/ntg_drawable.h"
#include "core/scene/shared/ntg_cdrawable_vec.h"
#include "core/scene/shared/ntg_drawable_vec.h"
#include "shared/ntg_log.h"

/* -------------------------------------------------------------------------- */
/* STATIC DECLARATIONS */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_scene* scene);
static void __add_to_registered_fn(ntg_drawable* drawable, void* _registered);
static void __update_focused(ntg_scene* scene);
static void __scan_scene(ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

void __ntg_scene_init__(
        ntg_scene* scene,
        ntg_scene_layout_fn layout_fn,
        ntg_scene_on_register_fn on_register_fn,
        ntg_scene_on_unregister_fn on_unregister_fn,
        ntg_scene_process_key_fn process_key_fn,
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
    scene->__process_key_fn = process_key_fn;
    scene->_data = data;

    scene->_delegate = ntg_event_delegate_new();
    scene->_graph = ntg_scene_graph_new();
}

void __ntg_scene_deinit__(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_event_delegate_destroy(scene->_delegate);
    ntg_scene_graph_destroy(scene->_graph);

    __init_default_values(scene);
}

ntg_drawable* ntg_scene_get_focused(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->_focused;
}

void ntg_scene_focus(ntg_scene* scene, ntg_drawable* drawable)
{
    assert(scene != NULL);

    scene->__pending_focused = drawable;
    scene->__pending_focused_flag = true;
}

struct ntg_scene_node ntg_scene_get_node(const ntg_scene* scene,
        const ntg_drawable* drawable)
{
    assert(scene != NULL);
    assert(drawable != NULL);

    struct _ntg_scene_node* _node = ntg_scene_graph_get(scene->_graph, drawable);
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

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene != NULL);

    scene->_size = size;

    __scan_scene(scene);

    __update_focused(scene);

    scene->__layout_fn(scene, size);
}

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->_size;
}

/* -------------------------------------------------------------------------- */

void ntg_scene_set_root(ntg_scene* scene, ntg_drawable* root)
{
    assert(scene != NULL);

    scene->_root = root;
}

ntg_drawable* ntg_scene_get_root(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->_root;
}

/* -------------------------------------------------------------------------- */

bool ntg_scene_feed_key_event(
        ntg_scene* scene,
        struct nt_key_event key,
        ntg_loop_context* loop_context)
{
    assert(scene != NULL);

    return scene->__process_key_fn(scene, key, loop_context);
}

ntg_listenable* ntg_scene_get_listenable(ntg_scene* scene)
{
    assert(scene != NULL);

    return ntg_event_delegate_listenable(scene->_delegate);
}

/* -------------------------------------------------------------------------- */
/* STATIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_scene* scene)
{
    (*scene) = (ntg_scene) {0};
}

static void __add_to_registered_fn(ntg_drawable* drawable, void* _registered)
{
    ntg_drawable_vec* registered = (ntg_drawable_vec*)_registered;

    ntg_drawable_vec_append(registered, drawable);
}

static void __update_focused(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_drawable* old = scene->_focused;
    ntg_drawable* new = scene->__pending_focused;

    if(scene->__pending_focused_flag)
    {
        if(old != NULL)
        {
            struct ntg_unfocus_context ctx = {
                .new = new,
                .scene = scene
            };

            // ntg_log_log("DADA %p", new->_on_focus_fn);
            old->_on_unfocus_fn(old, ctx);
        }

        if(new != NULL)
        {
            struct _ntg_scene_node* node = ntg_scene_graph_get(scene->_graph, new);
            assert(node != NULL);

            scene->_focused = new;

            struct ntg_focus_context focus_ctx = {
                .old = old,
                .scene = scene
            };

            new->_on_focus_fn(new, focus_ctx);

            scene->__pending_focused = NULL;
        }

        scene->__pending_focused_flag = false;
    }

}

static void __scan_scene(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_drawable_vec current;
    __ntg_drawable_vec_init__(&current);
    if(scene->_root != NULL)
    {
        ntg_drawable_tree_perform(
                scene->_root,
                NTG_DRAWABLE_PERFORM_TOP_DOWN,
                __add_to_registered_fn,
                &current);
    }
    
    ntg_cdrawable_vec alr_registered; 
    __ntg_cdrawable_vec_init__(&alr_registered);
    ntg_scene_graph_get_keys(scene->_graph, &alr_registered);

    size_t i;
    const ntg_drawable* it_drawable;
    for(i = 0; i < alr_registered._count; i++)
    {
        it_drawable = alr_registered._data[i];

        // Drawable removed from the scene
        if(!(ntg_drawable_vec_contains(&current, it_drawable)))
        {
            ntg_scene_graph_remove(scene->_graph, it_drawable);

            if(scene->__on_unregister_fn != NULL)
                scene->__on_unregister_fn(scene, it_drawable);
        }
    }

    for(i = 0; i < current._count; i++)
    {
        it_drawable = current._data[i];

        // Drawable added to the scene
        if(!(ntg_cdrawable_vec_contains(&alr_registered, it_drawable)))
        {
            ntg_scene_graph_add(scene->_graph, it_drawable);

            if(scene->__on_register_fn != NULL)
                scene->__on_register_fn(scene, it_drawable);
        }
    }

    __ntg_drawable_vec_deinit__(&current);
    __ntg_cdrawable_vec_deinit__(&alr_registered);
}
