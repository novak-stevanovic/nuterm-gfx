#ifndef _NTG_SIMPLE_SCENE_H_
#define _NTG_SIMPLE_SCENE_H_

#include "core/ntg_scene.h"

typedef struct ntg_simple_scene
{
    ntg_scene _base;
} ntg_simple_scene;

/* To specify default behavior for `ntg_scene_process_key_fn`,
 * look at ntg_scene.h */
void __ntg_simple_scene_init__(ntg_simple_scene* scene);
void __ntg_simple_scene_deinit__(ntg_simple_scene* scene);

ntg_simple_scene* ntg_simple_scene_new();
void ntg_simple_scene_destroy(ntg_simple_scene* scene);

#endif // _NTG_SIMPLE_SCENE_H_
