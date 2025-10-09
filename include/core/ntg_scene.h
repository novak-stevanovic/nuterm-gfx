#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "base/ntg_event.h"
#include "core/ntg_scene_drawing.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"

typedef struct ntg_object ntg_object;
typedef struct ntg_scene ntg_scene;

#define NTG_SCENE(scn_ptr) ((ntg_scene*)(scn_ptr))

typedef enum ntg_scene_key_intercept_order
{
    /* Calls ntg_scene_process_key_fn, then feeds the key event to the root object,
     * then the root's focused object, etc. */
    NTG_SCENE_KEY_INTERCEPT_ORDER_INTERCEPT_FIRST,

    /* Feeds the key event to the directly focused object, then its parent, etc.
     * lastly, calls ntg_scene_process_key_fn */
    NTG_SCENE_KEY_INTERCEPT_ORDER_PROCESS_FIRST,
} ntg_scene_key_intercept_order;

typedef enum ntg_scene_key_intercept_rule
{
    /* allows both the scene and not directly focused objects to intercept */
    NTG_SCENE_KEY_INTERCEPT_ALLOW_ALL,

    /* allows scene to intercept */
    NTG_SCENE_KEY_INTERCEPT_ALLOW_SCENE,

    /* allows not directly focused objects to intercept */
    NTG_SCENE_KEY_INTERCEPT_ALLOW_OBJECTS,

    /* forbids interception - only the focused object receives the event */
    NTG_SCENE_KEY_INTERCEPT_FORBID_ALL
} ntg_scene_key_intercept_rule;

typedef enum ntg_scene_key_consume_mode
{
    /* if an event is consumed, it is not fed to the next entity(scene or object) */
    NTG_SCENE_KEY_CONSUME_ONCE,
    NTG_SCENE_KEY_CONSUME_UNCONSTRAINED
} ntg_scene_key_consume_mode;

/* Returns if the scene processed the key event. */
typedef bool (*ntg_scene_process_key_fn)(ntg_scene* scene,
        struct nt_key_event key_event);
typedef void (*ntg_scene_on_object_register_fn)(ntg_scene* scene,
        ntg_object* object);
typedef void (*ntg_scene_on_object_unregister_fn)(ntg_scene* scene,
        ntg_object* object);

/* -------------------------------------------------------------------------- */

struct ntg_scene
{
    ntg_object* __root;
    ntg_scene_drawing __drawing;

    ntg_listenable __listenable;

    struct ntg_xy __size;

    ntg_scene_on_object_register_fn __on_object_register_fn;
    ntg_scene_on_object_unregister_fn __on_object_unregister_fn;
    ntg_scene_process_key_fn __process_key_fn;
    ntg_scene_key_intercept_order __key_intercept_order;
    ntg_scene_key_intercept_rule __key_intercept_rule;
    ntg_scene_key_consume_mode __key_consume_mode;

    bool __unfocused;
};

void __ntg_scene_init__(ntg_scene* scene, ntg_object* root);
void __ntg_scene_deinit__(ntg_scene* scene);

/* -------------------------------------------------------------------------- */

ntg_object* ntg_scene_get_focused(ntg_scene* scene);
void ntg_scene_focus(ntg_scene* scene, ntg_object* object);

void ntg_scene_set_key_intercept_order(ntg_scene* scene, ntg_scene_key_intercept_order order);
ntg_scene_key_intercept_order ntg_scene_get_key_intercept_order(const ntg_scene* scene);

void ntg_scene_set_key_intercept_rule(ntg_scene* scene, ntg_scene_key_intercept_rule rule);
ntg_scene_key_intercept_rule ntg_scene_get_key_intercept_rule(const ntg_scene* scene);

void ntg_scene_set_key_consume_mode(ntg_scene* scene, ntg_scene_key_consume_mode mode);
ntg_scene_key_consume_mode ntg_scene_get_key_consume_mode(const ntg_scene* scene);

void ntg_scene_set_process_key_fn(ntg_scene* scene,
        ntg_scene_process_key_fn process_key_fn);
void ntg_scene_set_on_register_object_fn(ntg_scene* scene,
        ntg_scene_on_object_register_fn on_object_register_fn);
void ntg_scene_set_on_unregister_object_fn(ntg_scene* scene,
        ntg_scene_on_object_unregister_fn on_object_unregister_fn);

/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene);

/* Should be called by a stage every time the scene should be resized. */
void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size);

/* Performs scene layout - tells the root to layout, then it re-draws
 * the scene drawing. */
void ntg_scene_layout(ntg_scene* scene);

/* Should be called whenever an object enters the scene */
void ntg_scene_register_object(ntg_scene* scene, ntg_object* object);
/* Should be called whenever an object exits the scene */
void ntg_scene_unregister_object(ntg_scene* scene, ntg_object* object);

/* -------------------------------------------------------------------------- */

const ntg_scene_drawing* ntg_scene_get_drawing(const ntg_scene* scene);
ntg_object* ntg_scene_get_root(ntg_scene* scene);

/* -------------------------------------------------------------------------- */

/* Invokes ntg_scene_process_key_fn */
bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event);

/* Event types raised by ntg_scene:
 * 1) NTG_ETYPE_SCENE_RESIZE,
 * 3) NTG_ETYPE_SCENE_FOCUSED_CHANGE,
 * 4) NTG_ETYPE_SCENE_LAYOUT,
 * 5) NTG_ETYPE_SCENE_OBJECT_REGISTER,
 * 6) NTG_ETYPE_SCENE_OBJECT_UNREGISTER
 */
void ntg_scene_listen(ntg_scene* scene, struct ntg_event_sub sub);
void ntg_scene_stop_listening(ntg_scene* scene, void* subscriber);

/* -------------------------------------------------------------------------- */

#endif // _NTG_SCENE_H_
