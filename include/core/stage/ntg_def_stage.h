#ifndef _NTG_DEF_STAGE_H_
#define _NTG_DEF_STAGE_H_

#include "core/stage/ntg_stage.h"

typedef struct ntg_def_stage ntg_def_stage;

struct ntg_def_stage
{
    ntg_stage __base;
};

void __ntg_def_stage_init__(ntg_def_stage* stage, ntg_scene* scene, void* data);
void __ntg_def_stage_deinit__(ntg_def_stage* stage);

void __ntg_def_stage_compose_fn(ntg_stage* _stage, struct ntg_xy size);

#endif // _NTG_DEF_STAGE_H_
