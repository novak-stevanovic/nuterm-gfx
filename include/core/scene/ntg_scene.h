#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_attach_policy;

/* -------------------------------------------------------------------------- */
/* SCENE */
/* -------------------------------------------------------------------------- */

struct ntg_scene_layer_node;

struct ntg_scene
{
    ntg_stage* _stage;

    ntg_object* _root;

    struct ntg_xy _size;

    bool _dirty;

    struct
    {
        bool (*__on_key_fn)(ntg_scene* scene, struct nt_key_event key);
        bool (*__on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse);
    };

    ntg_focus_manager* _fm;

    void* data;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_scene_init(ntg_scene* scene);
void ntg_scene_deinit(ntg_scene* scene);
void ntg_scene_deinit_(void* _scene);

void ntg_scene_mark_dirty(ntg_scene* scene);

ntg_object* ntg_scene_hit_test(
        ntg_scene* scene,
        struct ntg_xy pos,
        struct ntg_xy* out_object_pos);

size_t ntg_scene_collect_layers_by_z(
        ntg_scene* scene,
        ntg_object** out_layers,
        size_t cap);
void ntg_scene_set_root(ntg_scene* scene, ntg_object* root);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

// Default implementations. Dispatches to focus manager
bool ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key);
bool ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

bool ntg_scene_on_key(ntg_scene* scene, struct nt_key_event key);
void ntg_scene_set_on_key_fn(ntg_scene* scene,
        bool (*on_key_fn)(ntg_scene* scene, struct nt_key_event key));

bool ntg_scene_on_mouse(ntg_scene* scene, struct nt_mouse_event mouse);
void ntg_scene_set_on_mouse_fn(ntg_scene* scene,
        bool (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse));

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

void _ntg_scene_clean(ntg_scene* scene);
bool _ntg_scene_is_dirty(const ntg_scene* scene);

// Called by ntg_stage
void _ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size);
void _ntg_scene_layout(ntg_scene* scene, sarena* arena);

// Called internally by ntg_stage. Updates only the scene's state
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

void _ntg_scene_register(ntg_scene* scene, ntg_object* root);
void _ntg_scene_unregister(ntg_scene* scene, ntg_object* root);
void _ntg_scene_register_tree(ntg_scene* scene, ntg_object* root);
void _ntg_scene_unregister_tree(ntg_scene* scene, ntg_object* root);

#endif // NTG_SCENE_H
