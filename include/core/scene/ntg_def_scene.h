#ifndef _NTG_DEF_SCENE_H_
#define _NTG_DEF_SCENE_H_

#include "core/scene/ntg_scene.h"

typedef struct sarena sarena;
typedef struct ntg_def_scene ntg_def_scene;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* Default scene with implemented layout that allows for wrapping of elements.
 * Layout is always performed in full. */
struct ntg_def_scene
{
    ntg_scene __base;

    sarena* __layout_arena;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_scene* ntg_def_scene_new(ntg_entity_system* system);

void _ntg_def_scene_init_(ntg_def_scene* scene, ntg_focuser* focuser);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_scene_deinit_fn(ntg_entity* entity);

void _ntg_def_scene_layout_fn(ntg_scene* _scene, struct ntg_xy size);

#endif // _NTG_DEF_SCENE_H_
