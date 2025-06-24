#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "shared/ntg_xy.h"
#include "nt_event.h"

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
void ntg_scene_focus(ntg_scene* scene, ntg_object* object);

bool ntg_scene_feed_key_event(ntg_scene* scene, struct nt_key_event key_event);

#endif // _NTG_SCENE_H_
