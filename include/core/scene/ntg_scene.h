#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "core/scene/_ntg_scene_graph.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"

/* -------------------------------------------------------------------------- */
/* DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_object ntg_object;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_dlgt ntg_event_dlgt;
typedef struct ntg_loop_ctx ntg_loop_ctx;
struct ntg_event_sub;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* Performs logical layout of the scene. */
typedef void (*ntg_scene_layout_fn)(
        ntg_scene* scene,
        struct ntg_xy size);

/* Returns if the scene processed the key event.
 *
 * A default implementation of this fn exists - the scene will feed the key to
 * the focused object. */
typedef bool (*ntg_scene_process_key_fn)(
        ntg_scene* scene,
        struct nt_key_event key,
        ntg_loop_ctx* loop_ctx);

typedef void (*ntg_scene_on_register_fn)(
        ntg_scene* scene,
        const ntg_object* object);

typedef void (*ntg_scene_on_unregister_fn)(
        ntg_scene* scene,
        const ntg_object* object);

typedef void (*ntg_scene_deinit_fn)(ntg_scene* scene);

struct ntg_scene_node
{
    struct ntg_xy min_size, natural_size, max_size, grow;

    struct ntg_xy size;
    struct ntg_xy position;
    const ntg_object_drawing* drawing;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void _ntg_scene_init_(
        ntg_scene* scene, /* non-NULL */
        ntg_scene_layout_fn layout_fn, /* non-NULL */
        ntg_scene_on_register_fn on_register_fn,
        ntg_scene_on_unregister_fn on_unregister_fn,
        ntg_scene_process_key_fn process_key_fn,
        ntg_scene_deinit_fn deinit_fn);
void _ntg_scene_deinit_(ntg_scene* scene);

void _ntg_scene_vdeinit_(ntg_scene* scene);

/* ------------------------------------------------------ */

ntg_object* ntg_scene_get_focused(ntg_scene* scene);
void ntg_scene_focus(ntg_scene* scene, ntg_object* object);

ntg_object* ntg_scene_get_root(ntg_scene* scene);
void ntg_scene_set_root(ntg_scene* scene, ntg_object* root);

struct ntg_scene_node ntg_scene_get_node(const ntg_scene* scene,
        const ntg_object* object);

/* ------------------------------------------------------ */

/* Performs scene layout.
 *
 * First, updates the scene's size.
 *
 * Second, updates the scene graph calling scene's
 * `ntg_scene_on_register_fn` and `ntg_scene_on_unregister_fn` for each new and
 * removed object from the graph, respectively.
 *
 * Third, it updates the focused object based on pending_focused field.
 *
 * Finally, it calls the scene's `ntg_scene_layout_fn` to perform the layout. */
void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size);

struct ntg_xy ntg_scene_get_size(const ntg_scene* scene);

/* ------------------------------------------------------ */

bool ntg_scene_feed_key_event(
        ntg_scene* scene,
        struct nt_key_event key,
        ntg_loop_ctx* loop_ctx);

ntg_listenable* ntg_scene_get_listenable(ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_scene
{
    ntg_object* _root;
    struct ntg_xy _size;

    ntg_scene_graph* _graph;

    ntg_scene_layout_fn __layout_fn;
    ntg_scene_on_register_fn __on_register_fn;
    ntg_scene_on_unregister_fn __on_unregister_fn;
    ntg_scene_process_key_fn __process_key_fn;
    ntg_scene_deinit_fn __deinit_fn;

    ntg_object *_focused, *__pending_focused;
    bool __pending_focused_flag;

    ntg_event_dlgt* _delegate;

    void* data;
};

#endif // _NTG_SCENE_H_
