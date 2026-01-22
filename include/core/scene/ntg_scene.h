#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "core/entity/ntg_entity.h"
#include "shared/ntg_xy.h"
#include "core/object/widget/ntg_widget.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_scene
{
    ntg_entity __base;

    struct
    {
        ntg_stage* _stage;
    };

    struct
    {
        struct ntg_xy _size;
        ntg_widget* _root;
        ntg_scene_layout_fn __layout_fn;
    };

    struct
    {
        ntg_scene_process_fn __process_fn;
    };

    struct
    {
        ntg_widget_slist __focus_ctx_stack;
    };

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena);
void ntg_scene_set_root(ntg_scene* scene, ntg_widget* root);

void ntg_scene_focus_ctx_push(ntg_scene* scene, ntg_widget* widget);
void ntg_scene_focus_ctx_pop(ntg_scene* scene);

bool ntg_scene_feed_event(ntg_scene* scene, struct ntg_event event,
                          ntg_loop_ctx* ctx);

bool ntg_scene_dispatch_def(ntg_scene* scene, struct ntg_event event,
                            ntg_loop_ctx* ctx);
void ntg_scene_set_process_fn(ntg_scene* scene, ntg_scene_process_fn fn);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_scene_init(ntg_scene* scene, ntg_scene_layout_fn layout_fn);
void ntg_scene_deinit_fn(ntg_entity* entity);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

// Called internally by ntg_stage. Updates only the scene's state
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

#endif // NTG_SCENE_H
