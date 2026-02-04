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

struct ntg_dock_ctx
{
    ntg_orient orient;
    size_t layer_min_size, layer_nat_size, layer_max_size;
    size_t scene_size;
};

struct ntg_dock_plc
{
    void* data;
    void (*dock_fn)(
            void* data,
            struct ntg_dock_ctx ctx,
            size_t* out_size,
            size_t* out_pos,
            sarena* arena);
    void (*free_fn)(void* data);
};

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

    bool _dirty;

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

void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size);
void ntg_scene_mark_dirty(ntg_scene* scene);
void ntg_scene_layout(ntg_scene* scene, sarena* arena);
ntg_object* ntg_scene_hit_test(ntg_scene* scene, struct ntg_xy pos);

/* -------------------------------------------------------------------------- */
/* LAYER */
/* -------------------------------------------------------------------------- */

void ntg_scene_dock_layer(
        ntg_scene* scene,
        ntg_scene_layer* layer,
        ntg_scene_layer* base,
        struct ntg_dock_plc policy);

void ntg_scene_undock_layer(ntg_scene* scene, ntg_scene_layer* layer);

size_t ntg_scene_get_layer_count(const ntg_scene* scene);
void ntg_scene_get_layers_by_z(
        ntg_scene* scene,
        ntg_scene_layer** out_buff,
        size_t cap);

/* -------------------------------------------------------------------------- */
/* DOCK POLICY */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* ROOT */
/* ------------------------------------------------------ */

struct ntg_dock_plc ntg_dock_plc_root();

/* ------------------------------------------------------ */
/* DOCK */
/* ------------------------------------------------------ */

enum ntg_dock_plc_anchor_orient
{
    NTG_DOCK_PLC_ATTACH_ORIENT_N,
    NTG_DOCK_PLC_ATTACH_ORIENT_E,
    NTG_DOCK_PLC_ATTACH_ORIENT_S,
    NTG_DOCK_PLC_ATTACH_ORIENT_W,
    NTG_DOCK_PLC_ATTACH_ORIENT_C
};

enum ntg_dock_plc_anchor_size
{
    NTG_DOCK_PLC_ATTACH_SIZE_MEASURE,
    NTG_DOCK_PLC_ATTACH_SIZE_DOCK
};

enum ntg_dock_plc_anchor_thresh
{
    NTG_DOCK_PLC_ATTACH_THRESH_MIN,
    NTG_DOCK_PLC_ATTACH_THRESH_NAT,
    NTG_DOCK_PLC_ATTACH_THRESH_ALWAYS,
};

enum ntg_dock_plc_anchor_enable
{
    NTG_DOCK_PLC_ATTACH_ENABLE_STATIC,
    NTG_DOCK_PLC_ATTACH_ENABLE_DYNAMIC
};

struct ntg_dock_plc_anchor_dt
{
    ntg_scene_layer* base;
    ntg_object* dock;
    struct ntg_insets shrink;
    ntg_align align;
    enum ntg_dock_plc_anchor_orient orient;
    enum ntg_dock_plc_anchor_size size;
    enum ntg_dock_plc_anchor_thresh thresh;
    enum ntg_dock_plc_anchor_enable enable;
};

struct ntg_dock_plc
ntg_dock_plc_anchor(struct ntg_dock_plc_anchor_dt dt);

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

void _ntg_scene_clean(ntg_scene* scene);

// Called internally by ntg_stage. Updates only the scene's state
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

#endif // NTG_SCENE_H
