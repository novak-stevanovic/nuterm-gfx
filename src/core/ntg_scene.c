#include "core/ntg_scene.h"

#include <assert.h>
#include <stdlib.h>

#include "base/ntg_event_types.h"
#include "core/ntg_scene_drawing.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "shared/ntg_log.h"
#include "shared/ntg_xy.h"

static void __draw_scene_object_fn(ntg_object* object, void* _scene);
static void __construct_empty_drawing(ntg_scene* scene);

static bool __perform_intercept(ntg_scene* scene, struct nt_key_event key_event);

/* -------------------------------------------------------------------------- */

void __ntg_scene_init__(ntg_scene* scene, ntg_object* root)
{
    assert(scene != NULL);
    assert(root != NULL);

    scene->__process_key_fn = NULL;
    scene->__root = root;
    scene->__key_consume_mode = NTG_SCENE_KEY_CONSUME_ONCE;
    scene->__key_intercept_order = NTG_SCENE_KEY_INTERCEPT_ORDER_PROCESS_FIRST;
    scene->__key_intercept_rule = NTG_SCENE_KEY_INTERCEPT_ALLOW_SCENE;
    scene->__on_object_register_fn = NULL;
    scene->__on_object_unregister_fn =  NULL;
    scene->__size = NTG_XY_UNSET;
    scene->__unfocused = false;

    __ntg_scene_drawing_init__(&scene->__drawing);
    __ntg_listenable_init__(&scene->__listenable);
}

void __ntg_scene_deinit__(ntg_scene* scene)
{
    assert(scene != NULL);

    scene->__process_key_fn = NULL;
    scene->__root = NULL;
    scene->__size = NTG_XY_UNSET;
    scene->__on_object_register_fn = NULL;
    scene->__on_object_unregister_fn = NULL;

    __ntg_scene_drawing_deinit__(&scene->__drawing);
    __ntg_listenable_deinit__(&scene->__listenable);
}

ntg_object* ntg_scene_get_focused(ntg_scene* scene)
{
    assert(scene != NULL);

    if(scene->__unfocused == true) return NULL;

    ntg_object* it_object = scene->__root;

    while(ntg_object_get_focused(it_object) != NULL)
    {
        it_object = ntg_object_get_focused(it_object);
    }

    return it_object;
}

void ntg_scene_focus(ntg_scene* scene, ntg_object* object)
{
    assert(scene != NULL);

    if(object == NULL)
    {
        scene->__unfocused = true;
    }
    else
    {
        scene->__unfocused = false;

        assert(ntg_object_is_descendant(scene->__root, object));

        ntg_object* it_focused_object = object;
        ntg_object* it_object = ntg_object_get_parent(object,
                NTG_OBJECT_GET_PARENT_INCL_DECORATOR);

        while(it_object != NULL)
        {
            ntg_log_log("%p %p", it_object, it_focused_object);
            ntg_object_focus(it_object, it_focused_object);
            it_focused_object = it_object;
            it_object = ntg_object_get_parent(it_object,
                    NTG_OBJECT_GET_PARENT_INCL_DECORATOR);
        }
    }
}

void ntg_scene_set_key_intercept_order(ntg_scene* scene, ntg_scene_key_intercept_order order)
{
    assert(scene != NULL);

    scene->__key_intercept_order = order;
}

ntg_scene_key_intercept_order ntg_scene_get_key_intercept_order(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__key_intercept_order;
}

void ntg_scene_set_key_intercept_rule(ntg_scene* scene, ntg_scene_key_intercept_rule rule)
{
    assert(scene != NULL);

    scene->__key_intercept_rule = rule;
}

ntg_scene_key_intercept_rule ntg_scene_get_key_intercept_rule(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__key_intercept_rule;
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

void ntg_scene_set_process_key_fn(ntg_scene* scene,
        ntg_scene_process_key_fn process_key_fn)
{
    assert(scene != NULL);

    scene->__process_key_fn = process_key_fn;
}

void ntg_scene_set_on_register_object_fn(ntg_scene* scene,
        ntg_scene_on_object_register_fn on_object_register_fn)
{
    assert(scene != NULL);

    scene->__on_object_register_fn = on_object_register_fn;
}

void ntg_scene_set_on_unregister_object_fn(ntg_scene* scene,
        ntg_scene_on_object_unregister_fn on_object_unregister_fn)
{
    assert(scene != NULL);

    scene->__on_object_unregister_fn = on_object_unregister_fn;
}

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__size;
}

void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size)
{
    assert(scene != NULL);

    if(ntg_xy_are_equal(scene->__size, size)) return;

    scene->__size = size;
    ntg_scene_drawing_set_size(&scene->__drawing, size);
}

void ntg_scene_layout(ntg_scene* scene)
{
    assert(scene != NULL);

    if(scene->__root != NULL)
    {
        ntg_object_layout(scene->__root, scene->__size);
        ntg_object_tree_perform(scene->__root, NTG_OBJECT_PERFORM_TOP_DOWN,
                __draw_scene_object_fn, scene);
    }
    else // construct empty drawing
        __construct_empty_drawing(scene);
}

bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event)
{
    assert(scene != NULL);

    ntg_object* focused = ntg_scene_get_focused(scene);
    bool consumed = false;

    if(scene->__key_intercept_order == NTG_SCENE_KEY_INTERCEPT_ORDER_INTERCEPT_FIRST)
        consumed = __perform_intercept(scene, key_event);
    if(consumed && (scene->__key_consume_mode == NTG_SCENE_KEY_CONSUME_ONCE))
        return true;

    if(focused != NULL)
    {
        consumed = ntg_object_feed_key(focused, key_event, false, consumed);
        if(consumed && (scene->__key_consume_mode == NTG_SCENE_KEY_CONSUME_ONCE))
            return true;
    }

    if(scene->__key_intercept_order == NTG_SCENE_KEY_INTERCEPT_ORDER_PROCESS_FIRST)
        consumed = __perform_intercept(scene, key_event);

    return consumed;
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

const ntg_scene_drawing* ntg_scene_get_drawing(const ntg_scene* scene)
{
    assert(scene != NULL);

    return &(scene->__drawing);
}

ntg_object* ntg_scene_get_root(ntg_scene* scene)
{
    assert(scene != NULL);

    return scene->__root;
}

/* -------------------------------------------------------------------------- */

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

static void __draw_scene_object_fn(ntg_object* object, void* _scene)
{
    ntg_scene* scene = NTG_SCENE(_scene);

    const ntg_object_drawing* object_drawing =  ntg_object_get_drawing(object);
    ntg_scene_drawing* scene_drawing = &scene->__drawing;

    struct ntg_xy pos = ntg_object_get_position_abs(object);
    struct ntg_xy size = ntg_object_get_size(object);

    ntg_object_drawing_place_(object_drawing, ntg_xy(0, 0),
            size, scene_drawing, pos);
}

static void __construct_empty_drawing(ntg_scene* scene)
{
    size_t i, j;
    struct ntg_rcell* it_cell;
    for(i = 0; i < scene->__size.y; i++)
    {
        for(j = 0; j < scene->__size.x; j++)
        {
            it_cell = ntg_scene_drawing_at_(&scene->__drawing, ntg_xy(j, i));
            (*it_cell) = ntg_rcell_default();
        }
    }
}

static bool __perform_intercept(ntg_scene* scene, struct nt_key_event key_event)
{
    ntg_object* focused = ntg_scene_get_focused(scene);

    bool consumed = false;
    if((scene->__key_intercept_rule == NTG_SCENE_KEY_INTERCEPT_ALLOW_ALL) ||
            scene->__key_intercept_rule == NTG_SCENE_KEY_INTERCEPT_ALLOW_SCENE)
    {
        if(scene->__process_key_fn != NULL)
            consumed = scene->__process_key_fn(scene, key_event);
    }

    if((scene->__key_intercept_rule == NTG_SCENE_KEY_INTERCEPT_ALLOW_ALL) ||
            scene->__key_intercept_rule == NTG_SCENE_KEY_INTERCEPT_ALLOW_OBJECTS)
    {
        ntg_object* it_object = scene->__root;
        ntg_object_vec it_children;
        ntg_object_get_children_(it_object, &it_children);

        size_t i;
        while(it_object != focused)
        {
            if(scene->__key_consume_mode == NTG_SCENE_KEY_CONSUME_ONCE)
            {
                if(consumed) return true;
            }

            consumed = ntg_object_feed_key(it_object, key_event, true, consumed);

            // Find it_object and it_children
            for(i = 0; i < it_children._count; i++)
            {
                if(ntg_object_is_descendant(it_children._data[i], focused))
                {
                    it_object = it_children._data[i];
                    break;
                }
            }
            __ntg_object_vec_deinit__(&it_children);
            ntg_object_get_children_(it_object, &it_children);
        }
    }

    return consumed;
}
