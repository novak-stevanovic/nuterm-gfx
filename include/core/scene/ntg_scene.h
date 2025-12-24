#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "base/entity/ntg_entity.h"
#include "core/scene/_ntg_scene_graph.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"

/* -------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_object ntg_object;
typedef struct ntg_loop_ctx ntg_loop_ctx;

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

struct ntg_scene_node
{
    struct ntg_xy min_size, natural_size, max_size, grow;

    struct ntg_xy size;
    struct ntg_xy position;
    const ntg_object_drawing* drawing;
};

struct ntg_scene
{
    ntg_entity __base;
    ntg_object* _root;

    ntg_scene_graph* _graph;

    ntg_scene_layout_fn __layout_fn;
    ntg_scene_process_key_fn __process_key_fn;

    ntg_object *_focused, *__pending_focused;
    bool __pending_focused_flag;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_object* ntg_scene_get_focused(ntg_scene* scene);
void ntg_scene_focus(ntg_scene* scene, ntg_object* object);

ntg_object* ntg_scene_get_root(ntg_scene* scene);
void ntg_scene_set_root(ntg_scene* scene, ntg_object* root);

struct ntg_scene_node ntg_scene_get_node(const ntg_scene* scene,
        const ntg_object* object);

/* ------------------------------------------------------ */

/* 1) Scans the scene for new objects
 *
 * 2) It updates the focused object based on pending_focused field.
 *
 * 3) Calls the scene's `ntg_scene_layout_fn` to perform the layout. */
void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size);

/* ------------------------------------------------------ */

bool ntg_scene_feed_key_event(
        ntg_scene* scene,
        struct nt_key_event key,
        ntg_loop_ctx* loop_ctx);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_scene_init_(ntg_scene* scene,
        ntg_scene_layout_fn layout_fn, 
        ntg_scene_process_key_fn process_key_fn);

void _ntg_scene_deinit_fn(ntg_entity* entity);

#endif // _NTG_SCENE_H_
