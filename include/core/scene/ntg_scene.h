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
typedef struct ntg_focuser ntg_focuser;
struct ntg_loop_event;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* Performs logical layout of the scene. */
typedef void (*ntg_scene_layout_fn)(
        ntg_scene* scene,
        struct ntg_xy size);

typedef bool (*ntg_scene_event_fn)(
        ntg_scene* scene,
        struct ntg_loop_event event,
        ntg_loop_ctx* loop_ctx);

typedef enum ntg_scene_event_mode
{
    NTG_SCENE_EVENT_PROCESS_FIRST,
    NTG_SCENE_EVENT_DISPATCH_FIRST
} ntg_scene_event_mode;

struct ntg_scene_node
{
    bool exists;
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
    ntg_scene_layout_fn __layout_fn;

    ntg_focuser* _focuser;

    ntg_scene_event_mode __event_mode;
    ntg_scene_event_fn __event_fn;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size);
void ntg_scene_set_root(ntg_scene* scene, ntg_object* root);

struct ntg_scene_node ntg_scene_get_node(
        const ntg_scene* scene,
        const ntg_object* object);

bool ntg_scene_feed_event(
        ntg_scene* scene,
        struct ntg_loop_event event,
        ntg_loop_ctx* loop_ctx);

void ntg_scene_set_event_fn(ntg_scene* scene, ntg_scene_event_fn fn);
void ntg_scene_set_event_mode(ntg_scene* scene, ntg_scene_event_mode mode);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_scene_init_(ntg_scene* scene,
        ntg_focuser* focuser,
        ntg_scene_layout_fn layout_fn);
void _ntg_scene_deinit_fn(ntg_entity* entity);

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

#endif // _NTG_SCENE_H_
