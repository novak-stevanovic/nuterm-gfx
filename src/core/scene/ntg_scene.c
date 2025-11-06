#include "core/scene/ntg_scene.h"

#include <assert.h>
#include <stdlib.h>

#include "base/event/ntg_event.h"
#include "base/event/ntg_listenable.h"
#include "core/scene/shared/ntg_scene_drawing.h"

static void __init_default_values(ntg_scene* scene)
{
    (*scene) = (ntg_scene) {0};
}

void __ntg_scene_init__(
        ntg_scene* scene,
        ntg_drawable* root,
        ntg_scene_layout_fn layout_fn,
        ntg_scene_on_register_fn on_register_fn,
        ntg_scene_on_unregister_fn on_unregister_fn,
        ntg_scene_process_key_fn process_key_fn)
{
    assert(scene != NULL);
    assert(root != NULL);
    assert(layout_fn != NULL);

    __init_default_values(scene);

    scene->__root = root;
    scene->__drawing = ntg_scene_drawing_new();
    scene->__focused = NULL;

    scene->__layout_fn = layout_fn;
    scene->__on_register_fn = on_register_fn;
    scene->__on_unregister_fn = on_unregister_fn;
    scene->__process_key_fn = process_key_fn;

    scene->__listenable = ntg_listenable_new();
}

void __ntg_scene_deinit__(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_scene_drawing_destroy(scene->__drawing);
    scene->__drawing = NULL;

    ntg_listenable_destroy(scene->__listenable);
    scene->__listenable = NULL;

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

    scene->__focused = drawable;
}

void ntg_scene_set_key_process_order(ntg_scene* scene, ntg_scene_key_process_order order)
{
    assert(scene != NULL);
    
    scene->__key_process_order = order;
}

ntg_scene_key_process_order ntg_scene_get_key_process_order(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__key_process_order;
}

void ntg_scene_set_key_consume_mode(ntg_scene* scene, ntg_scene_key_consume_mode mode)
{
    assert(scene != NULL);

    scene->__key_consume_mode = mode;
}

ntg_scene_key_consume_mode ntg_scene_get_key_consume_mode(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__key_consume_mode;
}

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene != NULL);

    scene->__size = size;
    ntg_scene_drawing_set_size(scene->__drawing, size);

    // TODO:
}

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__size;
}

/* -------------------------------------------------------------------------- */

const ntg_scene_drawing* ntg_scene_get_drawing(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__drawing;
}

ntg_drawable* ntg_scene_get_root(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__root;
}

/* -------------------------------------------------------------------------- */

bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event)
{
    assert(scene != NULL);

    // TODO:
    assert(0);
}

void ntg_scene_listen(ntg_scene* scene, struct ntg_event_sub sub)
{
    assert(scene != NULL);

    ntg_listenable_listen(scene->__listenable, sub);
}

void ntg_scene_stop_listening(ntg_scene* scene, void* subscriber)
{
    assert(scene != NULL);

    ntg_listenable_stop_listening(scene->__listenable, subscriber);
}
