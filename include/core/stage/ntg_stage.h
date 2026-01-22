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

    ntg_stage_event_fn __event_fn;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size, sarena* arena);

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

bool ntg_stage_feed_event(ntg_stage* stage, struct ntg_event event,
                          ntg_loop_ctx* ctx);

void ntg_stage_set_event_fn(ntg_stage* stage, ntg_stage_event_fn fn);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_stage_init(ntg_stage* stage, ntg_stage_compose_fn compose_fn);
void ntg_stage_deinit_fn(ntg_entity* entity);

#endif // NTG_STAGE_H
