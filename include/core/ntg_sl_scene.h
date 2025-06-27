#ifndef _NTG_SL_SCENE_H_
#define _NTG_SL_SCENE_H_

#include "core/ntg_scene.h"

typedef struct ntg_sl_scene
{
    ntg_scene _base;
} ntg_sl_scene;

/* To specify default behavior for `ntg_scene_process_key_fn`,
 * look at ntg_scene.h */
void __ntg_sl_scene_init__(ntg_sl_scene* scene,
        ntg_scene_process_key_fn process_key_fn);
void __ntg_sl_scene_deinit__(ntg_sl_scene* scene);

ntg_sl_scene* ntg_sl_scene_new(ntg_scene_process_key_fn process_key_fn);
void ntg_sl_scene_destroy(ntg_sl_scene* scene);

#endif // _NTG_SL_SCENE_H_
