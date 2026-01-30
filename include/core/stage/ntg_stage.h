#ifndef NTG_STAGE_H
#define NTG_STAGE_H

#include "shared/ntg_shared.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_stage
{
    ntg_scene* _scene;

    bool __compose;
    ntg_stage_drawing _drawing;

    ntg_loop* _loop;

    struct
    {
        bool (*__on_key_fn)(ntg_stage* stage, struct nt_key_event key);
        bool (*__on_mouse_fn)(ntg_stage* stage, struct nt_mouse_event mouse);
    };

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_stage_init(ntg_stage* stage);
void ntg_stage_deinit(ntg_stage* stage);

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size, sarena* arena);

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

// Default implementations of `on_key_fn` and `on_mouse_fn`. Dispatches to scene
bool ntg_stage_dispatch_key(ntg_stage* stage, struct nt_key_event key);
bool ntg_stage_dispatch_mouse(ntg_stage* stage, struct nt_mouse_event mouse);

void ntg_stage_set_on_key_fn(ntg_stage* stage,
        bool (*on_key_fn)(ntg_stage* stage, struct nt_key_event key));
bool ntg_stage_on_key(ntg_stage* stage, struct nt_key_event key);

void ntg_stage_set_on_mouse_fn(ntg_stage* stage,
        bool (*on_mouse_fn)(ntg_stage* stage, struct nt_mouse_event mouse));
bool ntg_stage_on_mouse(ntg_stage* stage, struct nt_mouse_event mouse);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_stage_set_loop(ntg_stage* stage, ntg_loop* loop);

#endif // NTG_STAGE_H
