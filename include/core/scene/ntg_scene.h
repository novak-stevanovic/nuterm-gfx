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
/* ATTACH POLICY */
/* -------------------------------------------------------------------------- */

struct ntg_attach_constrain_ctx
{
    const ntg_object* attacher_layer;
    const ntg_object* base_layer;
    size_t scene_size;
};

struct ntg_attach_arrange_ctx
{
    const ntg_object* attacher_layer;
    const ntg_object* base_layer;
    struct ntg_xy size;
    struct ntg_xy scene_size;
};

struct ntg_attach_policy
{
    union
    {
        char data[128];
        long double _align;
    };
    size_t (*constrain_fn)(
            const void* data,
            ntg_orient orient,
            const struct ntg_attach_constrain_ctx* ctx,
            sarena* arena);
    struct ntg_xy (*arrange_fn)(
            const void* data,
            const struct ntg_attach_arrange_ctx* ctx,
            sarena* arena);
};

// Same
const struct ntg_attach_policy* ntg_attach_policy_root();

/* -------------------------------------------------------------------------- */
/* SCENE */
/* -------------------------------------------------------------------------- */

struct ntg_scene_layer_node;

struct ntg_scene
{
    ntg_stage* _stage;

    struct ntg_scene_layer_node* __tree_root;

    struct ntg_xy _size;

    // Scene doesn't mark itself dirty ever
    bool _dirty;

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

void ntg_scene_mark_dirty(ntg_scene* scene);
void ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size);
void ntg_scene_layout(ntg_scene* scene, sarena* arena);
ntg_object* ntg_scene_hit_test(ntg_scene* scene, struct ntg_xy pos);

/* -------------------------------------------------------------------------- */
/* ROOT */
/* -------------------------------------------------------------------------- */

void ntg_scene_attach_root(
        ntg_scene* scene,
        ntg_object* attacher_layer,
        ntg_object* base_layer,
        const struct ntg_attach_policy* policy);

void ntg_scene_detach_root(ntg_scene* scene, ntg_object* root);

size_t ntg_scene_get_root_count(const ntg_scene* scene);
void ntg_scene_get_roots_by_z(
        ntg_scene* scene,
        ntg_object** out_buff,
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

/* -------------------------------------------------------------------------- */
/* ATTACH POLICY */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* FLOAT */
/* ------------------------------------------------------ */

enum ntg_attach_policy_flt_enable
{
    NTG_ATTACH_POLICY_FLT_ENABLE_MIN,
    NTG_ATTACH_POLICY_FLT_ENABLE_BASE_MIN,
    NTG_ATTACH_POLICY_FLT_ENABLE_NAT,
    NTG_ATTACH_POLICY_FLT_ENABLE_BASE_NAT,
    NTG_ATTACH_POLICY_FLT_ENABLE_ALWAYS,
};

struct ntg_attach_policy_flt_opts
{
    enum ntg_attach_policy_flt_enable enable;

    struct ntg_insets shrink;
    ntg_align prim_align, sec_align;
};

struct ntg_attach_policy_flt_opts ntg_attach_policy_flt_opts_def();

// If `base` is not descendant_or_eq to base_root, attached layer will not appear
void ntg_attach_policy_init_flt(
        struct ntg_attach_policy* policy,
        ntg_object* base,
        const struct ntg_attach_policy_flt_opts* opts);

/* ------------------------------------------------------ */
/* SIDE FLOAT */
/* ------------------------------------------------------ */

enum ntg_attach_policy_sflt_orient
{
    NTG_ATTACH_POLICY_SFLT_ORIENT_N,
    NTG_ATTACH_POLICY_SFLT_ORIENT_E,
    NTG_ATTACH_POLICY_SFLT_ORIENT_S,
    NTG_ATTACH_POLICY_SFLT_ORIENT_W,
};

enum ntg_attach_policy_sflt_thresh
{
    NTG_ATTACH_POLICY_SFLT_THRESH_MIN,
    NTG_ATTACH_POLICY_SFLT_THRESH_NAT,
    NTG_ATTACH_POLICY_SFLT_THRESH_ALWAYS,
};

enum ntg_attach_policy_sflt_enable
{
    NTG_ATTACH_POLICY_SFLT_ENABLE_STATIC,
    NTG_ATTACH_POLICY_SFLT_ENABLE_DYNAMIC
};

struct ntg_attach_policy_sflt_opts
{
    ntg_align align;
    enum ntg_attach_policy_sflt_orient orient;
    enum ntg_attach_policy_sflt_thresh thresh;
    enum ntg_attach_policy_sflt_enable enable;
};

struct ntg_attach_policy_sflt_opts ntg_attach_policy_sflt_opts_def();

// If `base` is not descendant_or_eq to base_root, attached layer will not appear
void ntg_attach_policy_init_sflt(
        struct ntg_attach_policy* policy,
        ntg_object* base,
        const struct ntg_attach_policy_sflt_opts* opts);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

void _ntg_scene_clean(ntg_scene* scene);
bool _ntg_scene_is_dirty(const ntg_scene* scene);

// Called internally by ntg_stage. Updates only the scene's state
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

void _ntg_scene_register(ntg_scene* scene, ntg_object* root);
void _ntg_scene_unregister(ntg_scene* scene, ntg_object* root);
void _ntg_scene_register_tree(ntg_scene* scene, ntg_object* root);
void _ntg_scene_unregister_tree(ntg_scene* scene, ntg_object* root);

#endif // NTG_SCENE_H
