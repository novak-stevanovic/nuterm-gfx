#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "shared/ntg_xy.h"

typedef struct ntg_scene ntg_scene;

/* Forward declarations */
typedef struct ntg_scene_drawing ntg_scene_drawing;
typedef struct ntg_object ntg_object;

ntg_scene* ntg_scene_new();
void ntg_scene_destroy(ntg_scene* scene);

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size);

ntg_object* ntg_scene_get_root(const ntg_scene* scene);
void ntg_scene_set_root(ntg_scene* scene, ntg_object* new_root);

const ntg_scene_drawing* ntg_scene_get_drawing(const ntg_scene* scene);

ntg_scene_drawing* _ntg_scene_get_drawing(ntg_scene* scene);

ntg_object* ntg_scene_get_focused(const ntg_scene* scene);
void ntg_scene_set_focused(ntg_scene* scene, ntg_object* object);

#endif // _NTG_SCENE_H_
