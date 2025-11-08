#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "shared/ntg_xy.h"
#include "nt_event.h"

typedef struct ntg_scene_drawing ntg_scene_drawing;
typedef struct ntg_drawable_vec ntg_drawable_vec;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_listenable ntg_listenable;
struct ntg_event_sub;

#define NTG_SCENE(scn_ptr) ((ntg_scene*)(scn_ptr))

typedef enum ntg_scene_key_process_mode
{
    NTG_SCENE_KEY_PROCESS_FOCUSED_ONLY,
    NTG_SCENE_KEY_PROCESS_SCENE_ONLY,
    NTG_SCENE_KEY_PROCESS_FOCUSED_FIRST,
    NTG_SCENE_KEY_PROCESS_SCENE_FIRST,
} ntg_scene_key_process_mode;

/* Performs layout and updates the scene's drawing */
typedef void (*ntg_scene_layout_fn)(
        ntg_scene* scene,
        struct ntg_xy size);

/* Returns if the scene processed the key event. */
typedef bool (*ntg_scene_process_key_fn)(
        ntg_scene* scene,
        struct nt_key_event key_event);

typedef void (*ntg_scene_on_register_fn)(
        ntg_scene* scene,
        const ntg_drawable* drawable);

typedef void (*ntg_scene_on_unregister_fn)(
        ntg_scene* scene,
        const ntg_drawable* drawable);

/* -------------------------------------------------------------------------- */

struct ntg_scene
{
    ntg_drawable* __root;
    ntg_scene_drawing* __drawing;
    struct ntg_xy __size;

    ntg_drawable_vec* __registered;

    ntg_scene_layout_fn __layout_fn;
    ntg_scene_on_register_fn __on_register_fn;
    ntg_scene_on_unregister_fn __on_unregister_fn;
    ntg_scene_process_key_fn __process_key_fn;

    ntg_scene_key_process_mode __key_process_mode;

    ntg_drawable* __focused;

    ntg_listenable* __listenable;
};

void __ntg_scene_init__(
        ntg_scene* scene, /* non-NULL */
        ntg_drawable* root, /* non-NULL */
        ntg_scene_layout_fn layout_fn,
        ntg_scene_on_register_fn on_register_fn,
        ntg_scene_on_unregister_fn on_unregister_fn,
        ntg_scene_process_key_fn process_key_fn);
void __ntg_scene_deinit__(ntg_scene* scene);

/* -------------------------------------------------------------------------- */

ntg_drawable* ntg_scene_get_focused(ntg_scene* scene);
void ntg_scene_focus(ntg_scene* scene, ntg_drawable* drawable);

void ntg_scene_set_key_process_mode(ntg_scene* scene, ntg_scene_key_process_mode mode);
ntg_scene_key_process_mode ntg_scene_get_key_process_mode(const ntg_scene* scene);

/* -------------------------------------------------------------------------- */

/* Performs scene layout.
 *
 * First, updates the scene's size.
 *
 * Second, updates the scene graph calling scene's
 * `ntg_scene_on_register_fn` for each new drawable and `ntg_scene_on_unregister_fn`
 * for each removed drawable from the graph.
 *
 * Third, it calls the scene's `ntg_scene_layout_fn` to perform the layout.
 *
 * Finally, it draws all of the object's drawings onto the scene drawing. */
void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size);

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene);

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
