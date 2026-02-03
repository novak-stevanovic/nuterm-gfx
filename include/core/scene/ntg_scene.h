#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* SCOPE */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* LAYER */
/* -------------------------------------------------------------------------- */

struct ntg_scene_attach_ctx
{
    ntg_orient orient;
    size_t layer_min_size, layer_nat_size, layer_max_size;
    size_t scene_size;
};

struct ntg_scene_attach_policy
{
    void* data;
    void (*attach_fn)(
            void* data,
            struct ntg_scene_attach_ctx ctx,
            size_t* out_size,
            size_t* out_pos,
            sarena* arena);
    void (*free_fn)(void* data);
};

extern const struct ntg_scene_attach_policy NTG_SCENE_ATTACH_POLICY_ROOT;

/* -------------------------------------------------------------------------- */
/* SCENE */
/* -------------------------------------------------------------------------- */

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

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_scene_init(ntg_scene* scene);
void ntg_scene_deinit(ntg_scene* scene);
void ntg_scene_deinit_(void* _scene);

bool ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena);
ntg_object* ntg_scene_hit_test(ntg_scene* scene, struct ntg_xy pos);

/* -------------------------------------------------------------------------- */
/* LAYER */
/* -------------------------------------------------------------------------- */

void ntg_scene_attach_layer(
        ntg_scene* scene,
        ntg_scene_layer* layer,
        ntg_scene_layer* base,
        struct ntg_scene_attach_policy attach_policy);

void ntg_scene_detach_layer(ntg_scene* scene, ntg_scene_layer* layer);

size_t ntg_scene_get_layer_count(const ntg_scene* scene);
void ntg_scene_get_layers_by_z(
        ntg_scene* scene,
        ntg_scene_layer** out_buff,
        size_t cap);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key);
bool ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

void ntg_scene_set_on_key_fn(ntg_scene* scene,
        bool (*on_key_fn)(ntg_scene* scene, struct nt_key_event key));
bool ntg_scene_on_key(ntg_scene* scene, struct nt_key_event key);

void ntg_scene_set_on_mouse_fn(ntg_scene* scene,
        bool (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse));
bool ntg_scene_on_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

// Called internally by ntg_stage. Updates only the scene's state
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

#endif // NTG_SCENE_H
