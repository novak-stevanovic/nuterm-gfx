#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "shared/ntg_shared.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* SCOPE */
/* ------------------------------------------------------ */

/* ------------------------------------------------------ */
/* TREE */
/* ------------------------------------------------------ */

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

struct ntg_scene_priv;

struct ntg_scene
{
    struct
    {
        ntg_stage* _stage;
    };

    struct
    {
        struct ntg_xy _size;
    };

    struct ntg_scene_priv* __priv;

    struct
    {
        bool (*__on_key_fn)(ntg_scene* scene, struct nt_key_event key);
        bool (*__on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse);
    };

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_init(ntg_scene* scene);
void ntg_scene_deinit(ntg_scene* scene);

bool ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena);
ntg_object* ntg_scene_hit_test(ntg_scene* scene, struct ntg_xy pos);

/* ------------------------------------------------------ */
/* CTX */
/* ------------------------------------------------------ */

/* ------------------------------------------------------ */
/* FOCUS */
/* ------------------------------------------------------ */

/* ------------------------------------------------------ */
/* TREE */
/* ------------------------------------------------------ */

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key);
bool ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

void ntg_scene_set_on_key_fn(ntg_scene* scene,
        bool (*on_key_fn)(ntg_scene* scene, struct nt_key_event key));
bool ntg_scene_on_key(ntg_scene* scene, struct nt_key_event key);

void ntg_scene_set_on_mouse_fn(ntg_scene* scene,
        bool (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse));
bool ntg_scene_on_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

// Called internally by ntg_stage. Updates only the scene's state
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

#endif // NTG_SCENE_H
