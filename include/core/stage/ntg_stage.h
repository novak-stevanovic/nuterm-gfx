#ifndef NTG_STAGE_H
#define NTG_STAGE_H

#include "core/entity/ntg_entity.h"
#include "core/stage/shared/ntg_stage_drawing.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_stage
{
    ntg_entity __base;
    ntg_scene* _scene;

    ntg_stage_compose_fn __compose_fn;
    ntg_stage_drawing _drawing;

    ntg_loop* _loop;

    struct
    {
        bool (*on_key_fn)(ntg_stage* stage, struct nt_key_event key);
        bool (*on_mouse_fn)(ntg_stage* stage, struct nt_mouse_event mouse);
    };

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size, sarena* arena);

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

// Default implementations of `on_key_fn` and `on_mouse_fn`. Dispatches to scene
bool ntg_stage_dispatch_key(ntg_stage* stage, struct nt_key_event key);
bool ntg_stage_dispatch_mouse(ntg_stage* stage, struct nt_mouse_event mouse);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_stage_init(ntg_stage* stage, ntg_stage_compose_fn compose_fn);
void ntg_stage_deinit(ntg_stage* stage);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_stage_set_loop(ntg_stage* stage, ntg_loop* loop);

#endif // NTG_STAGE_H
