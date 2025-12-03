#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "core/scene/_ntg_scene_graph.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"

typedef struct ntg_drawable_vec ntg_drawable_vec;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef struct ntg_loop_context ntg_loop_context;
struct ntg_event_sub;

#define NTG_SCENE(scn_ptr) ((ntg_scene*)(scn_ptr))

typedef enum ntg_scene_process_key_mode
{
    NTG_SCENE_PROCESS_KEY_FOCUSED_FIRST,
    NTG_SCENE_PROCESS_KEY_SCENE_FIRST,
} ntg_scene_process_key_mode;

/* Performs logical layout of the scene. */
typedef void (*ntg_scene_layout_fn)(
        ntg_scene* scene,
        struct ntg_xy size);

/* Returns if the scene processed the key event. */
typedef bool (*ntg_scene_process_key_fn)(
        ntg_scene* scene,
        struct nt_key_event key_event,
        ntg_loop_context* loop_context);

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
    struct ntg_xy __size;

    ntg_scene_graph* _graph;

    ntg_scene_layout_fn __layout_fn;
    ntg_scene_on_register_fn __on_register_fn;
    ntg_scene_on_unregister_fn __on_unregister_fn;
    ntg_scene_process_key_fn __process_key_fn;

    ntg_scene_process_key_mode __process_key_mode;

    ntg_drawable *__focused, *__pending_focused;
    bool __pending_focused_flag;

    ntg_event_delegate* __del;

    void* _data;
};

struct ntg_scene_node
{
    struct ntg_xy min_size, natural_size,
                  max_size, grow;

    struct ntg_xy size;
    struct ntg_xy position;
    const ntg_drawing* drawing;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

void __ntg_scene_init__(
        ntg_scene* scene, /* non-NULL */
        ntg_scene_layout_fn layout_fn, /* non-NULL */
        ntg_scene_on_register_fn on_register_fn,
        ntg_scene_on_unregister_fn on_unregister_fn,
        ntg_scene_process_key_fn process_key_fn,
        void* data);
void __ntg_scene_deinit__(ntg_scene* scene);

/* -------------------------------------------------------------------------- */

ntg_drawable* ntg_scene_get_focused(ntg_scene* scene);
void ntg_scene_focus(ntg_scene* scene, ntg_drawable* drawable);

void ntg_scene_set_process_key_mode(ntg_scene* scene, ntg_scene_process_key_mode mode);
ntg_scene_process_key_mode ntg_scene_get_process_key_mode(const ntg_scene* scene);

struct ntg_scene_node ntg_scene_get_node(const ntg_scene* scene,
        const ntg_drawable* drawable);

/* -------------------------------------------------------------------------- */

/* Performs scene layout.
 *
 * First, updates the scene's size.
 *
 * Second, updates the scene graph calling scene's
 * `ntg_scene_on_register_fn` for each new drawable and `ntg_scene_on_unregister_fn`
 * for each removed drawable from the graph.
 *
 * Third, it updates the root if it was set beforehand(based on pending_root field).
 *
 * Finally, it calls the scene's `ntg_scene_layout_fn` to perform the layout. */
void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size);

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene);

/* -------------------------------------------------------------------------- */

void ntg_scene_set_root(ntg_scene* scene, ntg_drawable* root);
ntg_drawable* ntg_scene_get_root(ntg_scene* scene);

/* -------------------------------------------------------------------------- */

bool ntg_scene_feed_key_event(
        ntg_scene* scene,
        struct nt_key_event key_event,
        ntg_loop_context* loop_context);

/* -------------------------------------------------------------------------- */

ntg_listenable* ntg_scene_get_listenable(ntg_scene* scene);

#endif // _NTG_SCENE_H_
