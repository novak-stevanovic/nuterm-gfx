#ifndef NTG_DEF_STAGE_H
#define NTG_DEF_STAGE_H

#include <stdbool.h>
#include "shared/ntg_xy.h"
#include "core/stage/ntg_stage.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_def_stage
{
    ntg_stage __base;

    struct ntg_xy __old_size;
    bool __detected_changes;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_stage* ntg_def_stage_new(ntg_entity_system* system);
void ntg_def_stage_init(ntg_def_stage* stage, ntg_loop* loop);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_stage_deinit_fn(ntg_entity* entity);

void _ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size, sarena* arena);

#endif // NTG_DEF_STAGE_H
