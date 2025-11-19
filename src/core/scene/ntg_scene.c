#include "core/scene/ntg_scene.h"

#include <assert.h>
#include <stdlib.h>

#include "base/event/ntg_event.h"
#include "core/scene/ntg_drawable.h"
#include "core/scene/shared/ntg_cdrawable_vec.h"
#include "core/scene/shared/ntg_drawable_vec.h"

/* -------------------------------------------------------------------------- */
/* STATIC DECLARATIONS */
/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_scene* scene);
static void __add_to_registered_fn(ntg_drawable* drawable, void* _registered);
static void __scan_scene(ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

void __ntg_scene_init__(
        ntg_scene* scene,
        ntg_scene_layout_fn layout_fn,
        ntg_scene_on_register_fn on_register_fn,
        ntg_scene_on_unregister_fn on_unregister_fn,
        ntg_scene_process_key_fn process_key_fn)
{
    assert(scene != NULL);
    assert(layout_fn != NULL);

    __init_default_values(scene);

    scene->__root = NULL;
    scene->__focused = NULL;

    scene->__layout_fn = layout_fn;
    scene->__on_register_fn = on_register_fn;
    scene->__on_unregister_fn = on_unregister_fn;
    scene->__process_key_fn = process_key_fn;

    scene->__del = ntg_event_delegate_new();
    scene->__graph = ntg_scene_graph_new();
}

void __ntg_scene_deinit__(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_event_delegate_destroy(scene->__del);
    ntg_scene_graph_destroy(scene->__graph);

    __init_default_values(scene);
}

ntg_drawable* ntg_scene_get_focused(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__focused;
}

void ntg_scene_focus(ntg_scene* scene, ntg_drawable* drawable)
{
    assert(scene != NULL);

    // TODO: What if a user adds an element to the tree, the scene doesnt get
    // to register and then focuses the element?
    // assert(ntg_drawable_vec_contains(scene->__registered, drawable));

    scene->__focused = drawable;
}

void ntg_scene_set_key_process_mode(ntg_scene* scene, ntg_scene_key_process_mode mode)
{
    assert(scene != NULL);

    scene->__key_process_mode = mode;
}

ntg_scene_key_process_mode ntg_scene_get_key_process_mode(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__key_process_mode;
}

struct ntg_scene_node ntg_scene_get_node(const ntg_scene* scene,
        const ntg_drawable* drawable)
{
    assert(scene != NULL);
    assert(drawable != NULL);

    struct _ntg_scene_node* _node = ntg_scene_graph_get(scene->__graph, drawable);
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

    scene->__size = size;

    __scan_scene(scene);

    scene->__layout_fn(scene, size);
}

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__size;
}

/* -------------------------------------------------------------------------- */

void ntg_scene_set_root(ntg_scene* scene, ntg_drawable* root)
{
    assert(scene != NULL);

    scene->__root = root;
}

ntg_drawable* ntg_scene_get_root(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__root;
}

/* -------------------------------------------------------------------------- */

// TODO: TEST
bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event)
{
    assert(scene != NULL);

    ntg_drawable* focused = scene->__focused;
    struct ntg_process_key_context context = {
        .scene = scene
    };

    ntg_drawable_vec process_drawables;
    __ntg_drawable_vec_init__(&process_drawables);

    if(scene->__key_process_mode == NTG_SCENE_KEY_PROCESS_FOCUSED_ONLY)
    {
        if(focused != NULL)
            ntg_drawable_vec_append(&process_drawables, focused);
    }
    else if(scene->__key_process_mode == NTG_SCENE_KEY_PROCESS_FOCUSED_FIRST)
    {
        ntg_drawable* it_drawable = focused;

        while(it_drawable != NULL)
        {
            ntg_drawable_vec_append(&process_drawables, it_drawable);
            it_drawable = it_drawable->_get_parent_fn_(it_drawable);
        }

        ntg_drawable_vec_append(&process_drawables, (ntg_drawable*)scene);
    }
    else if(scene->__key_process_mode == NTG_SCENE_KEY_PROCESS_SCENE_ONLY)
    {
        ntg_drawable_vec_append(&process_drawables, (ntg_drawable*)scene);
    }
    else if(scene->__key_process_mode == NTG_SCENE_KEY_PROCESS_SCENE_FIRST)
    {
        ntg_drawable_vec_append(&process_drawables, (ntg_drawable*)scene);

        if(focused != NULL)
        {
            ntg_drawable* it_drawable = scene->__root;

            size_t i;
            ntg_drawable_vec_view it_children;
            ntg_drawable* it_child;
            while(it_drawable != NULL)
            {
                ntg_drawable_vec_append(&process_drawables, it_drawable);
                it_children = it_drawable->_get_children_fn_(it_drawable);

                for(i = 0; i < ntg_drawable_vec_view_count(&it_children); i++)
                {
                    it_child = ntg_drawable_vec_view_at(&it_children, i);

                    if(ntg_drawable_is_descendant(it_child, scene->__root))
                    {
                        it_drawable = it_child;
                    }
                }
            }
        }
    }
    else assert(0);

    bool processed = false;

    size_t i;
    ntg_drawable* it_drawable;
    for(i = 0; i < process_drawables._count; i++)
    {
        it_drawable = process_drawables._data[i];
        if(((ntg_scene*)it_drawable) == scene)
        {
            if(scene->__process_key_fn != NULL)
                processed |= scene->__process_key_fn(scene, key_event);
        }
        else
        {
            processed |= it_drawable->_process_key_fn(it_drawable,
                    key_event, context);
        }

        if(processed) break;
    }

    __ntg_drawable_vec_deinit__(&process_drawables);

    return processed;
}

ntg_listenable* ntg_scene_get_listenable(ntg_scene* scene)
{
    assert(scene != NULL);

    return ntg_event_delegate_listenable(scene->__del);
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

static void __scan_scene(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_drawable_vec current;
    __ntg_drawable_vec_init__(&current);
    if(scene->__root != NULL)
    {
        ntg_drawable_tree_perform(
                scene->__root,
                NTG_DRAWABLE_PERFORM_TOP_DOWN,
                __add_to_registered_fn,
                &current);
    }
    
    ntg_cdrawable_vec alr_registered; 
    __ntg_cdrawable_vec_init__(&alr_registered);
    ntg_scene_graph_get_keys(scene->__graph, &alr_registered);

    size_t i;
    const ntg_drawable* it_drawable;
    for(i = 0; i < alr_registered._count; i++)
    {
        it_drawable = alr_registered._data[i];

        // Drawable removed from the scene
        if(!(ntg_drawable_vec_contains(&current, it_drawable)))
        {
            ntg_scene_graph_remove(scene->__graph, it_drawable);

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
            ntg_scene_graph_add(scene->__graph, it_drawable);

            if(scene->__on_register_fn != NULL)
                scene->__on_register_fn(scene, it_drawable);
        }
    }

    __ntg_drawable_vec_deinit__(&current);
    __ntg_cdrawable_vec_deinit__(&alr_registered);
}

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

ntg_scene_graph* _ntg_scene_get_graph(ntg_scene* scene)
{
    return scene->__graph;
}
