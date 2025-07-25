#include "core/ntg_scene.h"
#include <assert.h>
#include <stdlib.h>

#include "core/ntg_scene_drawing.h"
#include "object/ntg_object.h"
#include "shared/ntg_xy.h"

static bool __process_key_fn_default(ntg_scene* scene,
        struct nt_key_event key_event);

/* -------------------------------------------------------------------------- */

void __ntg_scene_init__(ntg_scene* scene, ntg_scene_layout_fn layout_fn,
        ntg_scene_on_object_register_fn on_object_register_fn,
        ntg_scene_on_object_unregister_fn on_object_unregister_fn)
{
    assert(scene != NULL);

    scene->__process_key_fn = __process_key_fn_default;
    scene->__layout_fn = layout_fn;
    scene->_focused = NULL;
    scene->_root = NULL;
    scene->__on_object_register_fn = on_object_register_fn;
    scene->__on_object_unregister_fn = on_object_unregister_fn;
    __ntg_scene_drawing_init__(&scene->_drawing);
}

void __ntg_scene_deinit__(ntg_scene* scene)
{
    assert(scene != NULL);

    scene->__process_key_fn = NULL;
    scene->__layout_fn = NULL;
    scene->_focused = NULL;
    scene->_root = NULL;
    __ntg_scene_drawing_deinit__(&scene->_drawing);
}

void ntg_scene_set_root(ntg_scene* scene, ntg_object* new_root)
{
    assert(scene != NULL);

    if(scene->_root != NULL)
        _ntg_object_set_scene(scene->_root, NULL);

    scene->_root = new_root;

    if(new_root)
        _ntg_object_set_scene(new_root, scene);
}

void ntg_scene_focus(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);
    
    if(object != NULL)
        assert(scene == object->_scene);

    scene->_focused = object;
}

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene != NULL);

    ntg_scene_drawing_set_size(&scene->_drawing, size);

    scene->__layout_fn(scene, size);
}

bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event)
{
    assert(scene != NULL);

    return scene->__process_key_fn(scene, key_event);
}

void _ntg_scene_set_process_key_fn(ntg_scene* scene,
        ntg_scene_process_key_fn process_key_fn)
{
    assert(scene != NULL);
    assert(process_key_fn != NULL);

    scene->__process_key_fn = process_key_fn;
}

void ntg_scene_register_object(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    if(scene->__on_object_register_fn != NULL)
    {
        scene->__on_object_register_fn(scene, object);
    }
}

void ntg_scene_unregister_object(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    if(scene->__on_object_unregister_fn != NULL)
    {
        scene->__on_object_unregister_fn(scene, object);
    }
}

/* -------------------------------------------------------------------------- */

static bool __process_key_fn_default(ntg_scene* scene,
        struct nt_key_event key_event)
{
    assert(scene != NULL);

    if(scene->_focused != NULL)
        return ntg_object_feed_key(scene->_focused, key_event);
    else
        return false;
}

