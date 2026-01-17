#ifndef NTG_STAGE_H
#define NTG_STAGE_H

#include "core/entity/ntg_entity.h"
#include "core/stage/shared/ntg_stage_drawing.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

enum ntg_stage_event_mode
{
    NTG_STAGE_EVENT_PROCESS_FIRST,
    NTG_STAGE_EVENT_DISPATCH_FIRST
};

struct ntg_stage
{
    ntg_entity __base;
    ntg_loop* _loop;
    ntg_scene* _scene;

    ntg_stage_compose_fn __compose_fn;
    ntg_stage_drawing _drawing;

    ntg_stage_event_mode __event_mode;
    ntg_stage_event_fn __event_fn;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size, sarena* arena);
const ntg_stage_drawing* ntg_stage_get_drawing(const ntg_stage* stage);

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

bool ntg_stage_feed_event(
        ntg_stage* stage,
        struct nt_event event,
        ntg_loop_ctx* loop_ctx);

void ntg_stage_set_event_fn(ntg_stage* stage, ntg_stage_event_fn fn);
void ntg_stage_set_event_mode(ntg_stage* stage, ntg_stage_event_mode mode);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_stage_init(ntg_stage* stage, ntg_loop* loop,
        ntg_stage_compose_fn compose_fn);
void _ntg_stage_deinit_fn(ntg_entity* entity);

#endif // NTG_STAGE_H
