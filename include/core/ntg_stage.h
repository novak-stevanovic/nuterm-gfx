#ifndef _NTG_STAGE_H_
#define _NTG_STAGE_H_

#include "core/ntg_scene.h"
#include "nt_event.h"

void __ntg_stage_init__();
void __ntg_stage_deinit__();

void ntg_stage_set_scene(ntg_scene* scene);
void ntg_stage_render();

void ntg_stage_feed_event(struct nt_event event);

#endif // _NTG_STAGE_H_
