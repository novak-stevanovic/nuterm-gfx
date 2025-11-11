#ifndef _NTG_SIMPLE_SCENE_H_
#define _NTG_SIMPLE_SCENE_H_

#include "core/scene/ntg_scene.h"

typedef struct ntg_simple_scene ntg_simple_scene;

/* Simple scene with implemented layout that allows for wrapping of elements.
 * Layout is always performed in full. */
struct ntg_simple_scene
{
    ntg_scene __base;
};

void __ntg_simple_scene_init__(
        ntg_simple_scene* scene,
        ntg_scene_process_key_fn process_key_fn);
void __ntg_simple_scene_deinit__(ntg_simple_scene* scene);

#endif // _NTG_SIMPLE_SCENE_H_
