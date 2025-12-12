#ifndef _NTG_DEF_SCENE_H_
#define _NTG_DEF_SCENE_H_

#include "core/scene/ntg_scene.h"

typedef struct sarena sarena;
typedef struct ntg_def_scene ntg_def_scene;

/* Default scene with implemented layout that allows for wrapping of elements.
 * Layout is always performed in full. */
struct ntg_def_scene
{
    ntg_scene __base;

    sarena* __layout_arena;
};

void __ntg_def_scene_init__(
        ntg_def_scene* scene,
        ntg_scene_process_key_fn process_key_fn,
        void* data);
void __ntg_def_scene_deinit__(ntg_def_scene* scene);

void __ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size);

#endif // _NTG_DEF_SCENE_H_
