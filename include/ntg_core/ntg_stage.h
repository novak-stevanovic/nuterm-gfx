#ifndef _NTG_STAGE_H_
#define _NTG_STAGE_H_

#include "ntg_core/fwd/ntg_scene_fwd.h"

void __ntg_stage_init__();
void __ntg_stage_deinit__();

void ntg_stage_set_scene(ntg_scene_t* scene);
void ntg_stage_render();

#endif // _NTG_STAGE_H_
