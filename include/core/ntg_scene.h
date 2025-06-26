#ifndef _NTG_SCENE_H_
#define _NTG_SCENE_H_

#include "core/ntg_scene_drawing.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"

// TODO:

typedef struct ntg_object ntg_object;
typedef struct ntg_scene_engine ntg_scene_engine;
typedef struct ntg_scene ntg_scene;

struct ntg_scene
{
    ntg_object* _root;
    ntg_scene_drawing _drawing;

    ntg_scene_engine* _engine;

    ntg_object* _focused;
};

void __ntg_scene_init__();
void __ntg_scene_deinit__();

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

void ntg_scene_feed_resize_event(ntg_scene* scene, struct nt_resize_event resize_event);

#endif // _NTG_SCENE_H_
