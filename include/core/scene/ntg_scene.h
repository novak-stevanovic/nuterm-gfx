#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "base/entity/ntg_entity.h"
#include "core/scene/_ntg_scene_graph.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"

/* -------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_scene ntg_scene;
typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_object ntg_object;
typedef struct ntg_stage ntg_stage;
typedef struct ntg_loop_ctx ntg_loop_ctx;
struct ntg_loop_event;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* Performs logical layout of the scene. */
typedef void (*ntg_scene_layout_fn)(
        ntg_scene* scene,
        struct ntg_xy size);

typedef bool (*ntg_scene_process_event_fn)(
        ntg_scene* scene,
        struct ntg_loop_event event,
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

    ntg_stage* _stage;

    ntg_object* _root;
    ntg_scene_graph* _graph;
    ntg_object *_focused;

    ntg_scene_layout_fn __layout_fn;

    ntg_scene_process_event_fn __process_event_fn;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_focus(ntg_scene* scene, ntg_object* object);

ntg_object* ntg_scene_get_root(ntg_scene* scene);
void ntg_scene_set_root(ntg_scene* scene, ntg_object* root);

struct ntg_scene_node ntg_scene_get_node(const ntg_scene* scene,
        const ntg_object* object);

/* ------------------------------------------------------ */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size);

/* ------------------------------------------------------ */

bool ntg_scene_feed_event(
        ntg_scene* scene,
        struct ntg_loop_event event,
        ntg_loop_ctx* loop_ctx);

void ntg_scene_set_process_event_fn(ntg_scene* scene,
        ntg_scene_process_event_fn process_event_fn);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_scene_init_(ntg_scene* scene, ntg_scene_layout_fn layout_fn);

void _ntg_scene_deinit_fn(ntg_entity* entity);

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

#endif // _NTG_SCENE_H_
