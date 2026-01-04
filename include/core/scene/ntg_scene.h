#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "core/entity/ntg_entity.h"
#include "base/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

enum ntg_scene_event_mode
{
    NTG_SCENE_EVENT_PROCESS_FIRST,
    NTG_SCENE_EVENT_DISPATCH_FIRST
};

struct ntg_scene_hooks
{
    void* (*graph_node_data_init_fn)(ntg_scene* scene, ntg_object* object);
    void (*graph_node_data_deinit_fn)(ntg_scene* scene, ntg_object* object,
            void* data);
};

struct ntg_scene_node
{
    bool exists;
    struct ntg_xy min_size, natural_size, max_size, grow;

    struct ntg_xy size;
    struct ntg_xy rel_pos, abs_pos;
    const ntg_object_drawing* drawing;
};

struct ntg_scene
{
    ntg_entity __base;

    struct
    {
        ntg_stage* _stage;
    };

    struct
    {
        ntg_object* _root;
        ntg_scene_graph* __graph;
        ntg_scene_layout_fn __layout_fn;
    };

    struct
    {
        ntg_focuser* _focuser;

        ntg_scene_event_mode __event_mode;
        ntg_scene_event_fn __event_fn;
    };

    struct ntg_scene_hooks __hooks;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena);
void ntg_scene_set_root(ntg_scene* scene, ntg_object* root);

struct ntg_scene_node ntg_scene_get_node(
        const ntg_scene* scene,
        const ntg_object* object);

bool ntg_scene_feed_event(
        ntg_scene* scene,
        struct nt_event event,
        ntg_loop_ctx* loop_ctx);

void ntg_scene_set_event_fn(ntg_scene* scene, ntg_scene_event_fn fn);
void ntg_scene_set_event_mode(ntg_scene* scene, ntg_scene_event_mode mode);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_scene_init_(ntg_scene* scene,
        ntg_focuser* focuser,
        ntg_scene_layout_fn layout_fn,
        struct ntg_scene_hooks hooks);
void _ntg_scene_deinit_fn(ntg_entity* entity);

ntg_scene_graph* _ntg_scene_get_graph(ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

#endif // _NTG_SCENE_H_
