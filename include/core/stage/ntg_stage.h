#ifndef _NTG_STAGE_H_
#define _NTG_STAGE_H_

#include "base/entity/ntg_entity.h"
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_stage ntg_stage;
typedef struct ntg_scene ntg_scene;
typedef struct ntg_stage_drawing ntg_stage_drawing;
typedef struct ntg_loop_ctx ntg_loop_ctx;
struct nt_key_event;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* Composes the scene's logical drawing into an ntg_stage_drawing */
typedef void (*ntg_stage_compose_fn)(
        ntg_stage* stage,
        struct ntg_xy size);

struct ntg_stage
{
    ntg_entity __base;
    ntg_scene* _scene;
    ntg_stage_compose_fn __compose_fn;
    ntg_stage_drawing* _drawing;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_stage_compose(ntg_stage* stage, struct ntg_xy size);

const ntg_stage_drawing* ntg_stage_get_drawing(const ntg_stage* stage);

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);
ntg_scene* ntg_stage_get_scene(ntg_stage* stage);

bool ntg_stage_feed_key_event(
        ntg_stage* stage,
        struct nt_key_event key_event,
        ntg_loop_ctx* loop_ctx);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_stage_init_(ntg_stage* stage, ntg_stage_compose_fn compose_fn);

void _ntg_stage_deinit_fn(ntg_entity* entity);

#endif // _NTG_STAGE_H_
