#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "base/ntg_event.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"

typedef struct ntg_scene_drawing ntg_scene_drawing;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_drawable ntg_drawable;

#define NTG_SCENE(scn_ptr) ((ntg_scene*)(scn_ptr))

typedef enum ntg_scene_key_process_order
{
    /* Calls ntg_scene_process_key_fn, then feeds the key event to the root drawable,
     * then the root's focused drawable, etc. */
    NTG_SCENE_KEY_INTERCEPT_ORDER_INTERCEPT_FIRST,

    /* Feeds the key event to the directly focused drawable, then its parent, etc.
     * lastly, calls ntg_scene_process_key_fn */
    NTG_SCENE_KEY_INTERCEPT_ORDER_PROCESS_FIRST,
} ntg_scene_key_process_order;

typedef enum ntg_scene_key_consume_mode
{
    /* if an event is consumed, it is not fed to the next entity(scene or drawable) */
    NTG_SCENE_KEY_CONSUME_ONCE,
    NTG_SCENE_KEY_CONSUME_UNCONSTRAINED
} ntg_scene_key_consume_mode;

/* Returns if the scene processed the key event. */
typedef bool (*ntg_scene_process_key_fn)(ntg_scene* scene,
        struct nt_key_event key_event);
typedef void (*ntg_scene_on_register_fn)(ntg_scene* scene,
        const ntg_drawable* drawable);
typedef void (*ntg_scene_on_unregister_fn)(ntg_scene* scene,
        const ntg_drawable* drawable);

/* -------------------------------------------------------------------------- */

struct ntg_scene
{
    ntg_drawable* __root;
    ntg_scene_drawing* __drawing;

    ntg_listenable __listenable;

    struct ntg_xy __size;

    ntg_scene_on_register_fn __on_register_fn;
    ntg_scene_on_unregister_fn __on_unregister_fn;
    ntg_scene_process_key_fn __process_key_fn;
    ntg_scene_key_process_order __key_intercept_order;
    ntg_scene_key_consume_mode __key_consume_mode;

    ntg_drawable* __focused;
};

void __ntg_scene_init__(ntg_scene* scene, ntg_drawable* root);
void __ntg_scene_deinit__(ntg_scene* scene);

/* -------------------------------------------------------------------------- */

ntg_drawable* ntg_scene_get_focused(ntg_scene* scene);
void ntg_scene_focus(ntg_scene* scene, ntg_drawable* drawable);

void ntg_scene_set_key_process_order(ntg_scene* scene, ntg_scene_key_process_order order);
ntg_scene_key_process_order ntg_scene_get_key_process_order(const ntg_scene* scene);

void ntg_scene_set_key_consume_mode(ntg_scene* scene, ntg_scene_key_consume_mode mode);
ntg_scene_key_consume_mode ntg_scene_get_key_consume_mode(const ntg_scene* scene);

/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene);

/* Should be called by a stage every time the scene should be resized. */
void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size);

/* Performs scene layout - tells the root to layout, then it re-draws
 * the scene drawing. */
void ntg_scene_layout(ntg_scene* scene);

/* Should be called whenever an drawable enters the scene */
void ntg_scene_register_drawable(ntg_scene* scene, ntg_drawable* drawable);
/* Should be called whenever an drawable exits the scene */
void ntg_scene_unregister_drawable(ntg_scene* scene, ntg_drawable* drawable);

/* -------------------------------------------------------------------------- */

const ntg_scene_drawing* ntg_scene_get_drawing(const ntg_scene* scene);
ntg_drawable* ntg_scene_get_root(ntg_scene* scene);

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
