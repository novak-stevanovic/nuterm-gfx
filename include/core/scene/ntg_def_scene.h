#ifndef NTG_DEF_SCENE_H
#define NTG_DEF_SCENE_H

#include "core/scene/ntg_scene.h"
#include <stdbool.h>
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* Default scene with implemented layout that allows for wrapping of elements.
Optimized layout. */
struct ntg_def_scene
{
    ntg_scene __base;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_scene* ntg_def_scene_new(ntg_entity_system* system);

void ntg_def_scene_init(ntg_def_scene* scene);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_def_scene_deinit(ntg_def_scene* scene);

bool _ntg_def_scene_layout_fn(
        ntg_scene* _scene,
        struct ntg_xy size,
        sarena* arena);

#endif // NTG_DEF_SCENE_H
