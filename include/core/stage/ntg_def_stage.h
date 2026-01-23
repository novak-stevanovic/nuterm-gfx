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

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_stage* ntg_def_stage_new(ntg_entity_system* system);
void ntg_def_stage_init(ntg_def_stage* stage);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_def_stage_deinit(ntg_def_stage* stage);

void _ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size,
                               sarena* arena);

#endif // NTG_DEF_STAGE_H
