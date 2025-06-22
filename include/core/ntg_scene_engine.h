#ifndef _NTG_SCENE_ENGINE_H_
#define _NTG_SCENE_ENGINE_H_

#include "core/ntg_scene.h"

typedef struct ntg_scene_engine ntg_scene_engine;

ntg_scene_engine* ntg_scene_engine_new(ntg_scene* scene);
void ntg_scene_engine_destroy(ntg_scene_engine* engine);

void ntg_scene_engine_layout(ntg_scene_engine* engine);

#endif // _NTG_SCENE_ENGINE_H_
