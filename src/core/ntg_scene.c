#include "core/ntg_scene.h"
#include <assert.h>
#include <stdlib.h>

#include "base/ntg_event_types.h"
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
    scene->_size = NTG_XY_UNSET;

    __ntg_scene_drawing_init__(&scene->_drawing);
    __ntg_listenable_init__(&scene->__listenable);
}

void __ntg_scene_deinit__(ntg_scene* scene)
{
    assert(scene != NULL);

    scene->__process_key_fn = NULL;
    scene->__layout_fn = NULL;
    scene->_focused = NULL;
    scene->_root = NULL;
    scene->_size = NTG_XY_UNSET;

    __ntg_scene_drawing_deinit__(&scene->_drawing);
    __ntg_listenable_deinit__(&scene->__listenable);
}

void ntg_scene_set_root(ntg_scene* scene, ntg_object* new_root)
{
    assert(scene != NULL);

    if(scene->_root != NULL)
        _ntg_object_set_scene(scene->_root, NULL);

    if(scene->_root != new_root)
    {
        struct ntg_object_change data = {
            .old = scene->_root,
            .new = new_root
        };

        ntg_event e;
        __ntg_event_init__(&e, NTG_SCENE_ROOT_CHANGE, scene, &data);
        ntg_listenable_raise(&scene->__listenable, &e);
    }

    scene->_root = new_root;

    if(new_root)
        _ntg_object_set_scene(new_root, scene);
}

void ntg_scene_focus(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);

    if(scene->_focused != object)
    {
        struct ntg_object_change data = {
            .old = scene->_focused,
            .new = object
        };

        ntg_event e;
        __ntg_event_init__(&e, NTG_SCENE_FOCUSED_CHANGE, scene, &data);
        ntg_listenable_raise(&scene->__listenable, &e);
    }
    
    if(object != NULL)
        assert(scene == object->_scene);

    scene->_focused = object;
}

void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene != NULL);

    if(!ntg_xy_are_equal(scene->_size, size))
    {
        struct ntg_size_change data = {
            .old = scene->_size,
            .new = size
        };

        scene->_size = size;
        ntg_scene_drawing_set_size(&scene->_drawing, size);

        ntg_event e;
        __ntg_event_init__(&e, NTG_STAGE_RESIZE, scene, &data);
        ntg_listenable_raise(&scene->__listenable, &e);
    }
}

void ntg_scene_layout(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_event e;
    __ntg_event_init__(&e, NTG_SCENE_LAYOUT, scene, NULL);
    ntg_listenable_raise(&scene->__listenable, &e);

    scene->__layout_fn(scene);
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

    ntg_event e;
    __ntg_event_init__(&e, NTG_SCENE_OBJECT_REGISTER, scene, object);
    ntg_listenable_raise(&scene->__listenable, &e);

    if(scene->__on_object_register_fn != NULL)
    {
        scene->__on_object_register_fn(scene, object);
    }
}

void ntg_scene_unregister_object(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    ntg_event e;
    __ntg_event_init__(&e, NTG_SCENE_OBJECT_UNREGISTER, scene, object);
    ntg_listenable_raise(&scene->__listenable, &e);

    if(scene->__on_object_unregister_fn != NULL)
    {
        scene->__on_object_unregister_fn(scene, object);
    }
}

void ntg_scene_listen(ntg_scene* scene, struct ntg_event_sub sub)
{
    assert(scene != NULL);

    ntg_listenable_listen(&scene->__listenable, sub);
}

void ntg_scene_stop_listening(ntg_scene* scene, void* subscriber)
{
    assert(scene != NULL);

    ntg_listenable_stop_listening(&scene->__listenable, subscriber);
}

ntg_listenable* _ntg_scene_get_listenable(ntg_scene* scene)
{
    assert(scene != NULL);

    return &scene->__listenable;
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
