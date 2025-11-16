#ifndef _NTG_SIMPLE_STAGE_H_
#define _NTG_SIMPLE_STAGE_H_

#include "core/stage/ntg_stage.h"

typedef struct ntg_simple_stage ntg_simple_stage;

struct ntg_simple_stage
{
    ntg_stage __base;
};

void __ntg_simple_stage_init__(ntg_simple_stage* stage, ntg_scene* scene);
void __ntg_simple_stage_deinit__(ntg_simple_stage* stage);

void __ntg_simple_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size);

#endif // _NTG_SIMPLE_STAGE_H_
