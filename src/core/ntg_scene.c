#include "core/ntg_scene.h"
#include <assert.h>
#include <stdlib.h>

#include "base/ntg_event_types.h"
#include "core/ntg_scene_drawing.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/ntg_xy.h"

static void __draw_scene_object_fn(ntg_object* object, void* _scene);
static void __construct_empty_drawing(ntg_scene* scene);

static bool __process_key_fn_default(ntg_scene* scene,
        struct nt_key_event key_event);
static void __on_object_register_fn_default(ntg_scene* scene, ntg_object* object);
static void __on_object_unregister_fn_default(ntg_scene* scene, ntg_object* object);

/* -------------------------------------------------------------------------- */

void __ntg_scene_init__(ntg_scene* scene, ntg_object* root,
        ntg_scene_process_key_fn process_key_fn,
        ntg_scene_on_object_register_fn on_object_register_fn,
        ntg_scene_on_object_unregister_fn on_object_unregister_fn)
{
    assert(scene != NULL);
    assert(root != NULL);

    scene->__process_key_fn = (process_key_fn != NULL) ?
        process_key_fn : __process_key_fn_default;
    scene->_focused = NULL;
    scene->_root = root;
    scene->__on_object_register_fn = (on_object_register_fn != NULL) ?
        on_object_register_fn : __on_object_register_fn_default;
    scene->__on_object_unregister_fn = (on_object_unregister_fn != NULL) ?
        on_object_unregister_fn : __on_object_unregister_fn_default;
    scene->_size = NTG_XY_UNSET;

    __ntg_scene_drawing_init__(&scene->_drawing);
    __ntg_listenable_init__(&scene->__listenable);
}

void __ntg_scene_deinit__(ntg_scene* scene)
{
    assert(scene != NULL);

    scene->__process_key_fn = NULL;
    scene->_focused = NULL;
    scene->_root = NULL;
    scene->_size = NTG_XY_UNSET;
    scene->__on_object_register_fn = NULL;
    scene->__on_object_unregister_fn = NULL;

    __ntg_scene_drawing_deinit__(&scene->_drawing);
    __ntg_listenable_deinit__(&scene->__listenable);
}

void ntg_scene_focus(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);

    if(scene->_focused == object) return;

    if(object != NULL)
        assert(scene == ntg_object_get_scene(object));

    struct ntg_object_change data = {
        .old = scene->_focused,
        .new = object
    };

    scene->_focused = object;

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_SCENE_FOCUSED_CHANGE, scene, &data);
    ntg_listenable_raise(&scene->__listenable, &e);
}

void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene != NULL);

    if(ntg_xy_are_equal(scene->_size, size)) return;

    struct ntg_size_change data = {
        .old = scene->_size,
        .new = size
    };

    scene->_size = size;
    ntg_scene_drawing_set_size(&scene->_drawing, size);

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_STAGE_RESIZE, scene, &data);
    ntg_listenable_raise(&scene->__listenable, &e);
}

void ntg_scene_layout(ntg_scene* scene)
{
    assert(scene != NULL);

    if(scene->_root != NULL)
    {
        ntg_object_layout(scene->_root, scene->_size);
        ntg_object_perform_tree(scene->_root, NTG_OBJECT_PERFORM_TOP_DOWN,
                __draw_scene_object_fn, scene);
    }
    else // construct empty drawing
        __construct_empty_drawing(scene);

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_SCENE_LAYOUT, scene, NULL);
    ntg_listenable_raise(&scene->__listenable, &e);
}

bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event)
{
    assert(scene != NULL);
    
    if(scene->__process_key_fn == NULL) return false;

    return scene->__process_key_fn(scene, key_event);
}

void ntg_scene_register_object(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    if(scene->__on_object_register_fn != NULL)
    {
        scene->__on_object_register_fn(scene, object);
    }

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_SCENE_OBJECT_REGISTER, scene, object);
    ntg_listenable_raise(&scene->__listenable, &e);
}

void ntg_scene_unregister_object(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);
    assert(object != NULL);

    if(scene->__on_object_unregister_fn != NULL)
    {
        scene->__on_object_unregister_fn(scene, object);
    }

    ntg_event e;
    __ntg_event_init__(&e, NTG_ETYPE_SCENE_OBJECT_UNREGISTER, scene, object);
    ntg_listenable_raise(&scene->__listenable, &e);
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

/* -------------------------------------------------------------------------- */

ntg_listenable* _ntg_scene_get_listenable(ntg_scene* scene)
{
    assert(scene != NULL);

    return &scene->__listenable;
}

/* -------------------------------------------------------------------------- */

static void __draw_scene_object_fn(ntg_object* object, void* _scene)
{
    ntg_scene* scene = NTG_SCENE(_scene);

    const ntg_object_drawing* object_drawing =  ntg_object_get_drawing(object);
    ntg_scene_drawing* scene_drawing = &scene->_drawing;

    struct ntg_xy pos = ntg_object_get_position_abs(object);
    struct ntg_xy size = ntg_object_get_size(object);

    ntg_object_drawing_place_(object_drawing, ntg_xy(0, 0),
            size, scene_drawing, pos);
}

static void __construct_empty_drawing(ntg_scene* scene)
{
    size_t i, j;
    struct ntg_rcell* it_cell;
    for(i = 0; i < scene->_size.y; i++)
    {
        for(j = 0; j < scene->_size.x; j++)
        {
            it_cell = ntg_scene_drawing_at_(&scene->_drawing, ntg_xy(j, i));
            (*it_cell) = ntg_rcell_default();
        }
    }
}

static bool __process_key_fn_default(ntg_scene* scene,
        struct nt_key_event key_event)
{
    assert(scene != NULL);

    if(scene->_focused != NULL)
        return ntg_object_feed_key(scene->_focused, key_event);
    else
        return false;
}

static void __on_object_register_fn_default(ntg_scene* scene, ntg_object* object)
{
}

static void __on_object_unregister_fn_default(ntg_scene* scene, ntg_object* object)
{
}
