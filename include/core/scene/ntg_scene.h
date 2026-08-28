#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "shared/ntg_shared.h"
#include "core/scene/ntg_fcs_scope.h"
#include "core/widget/ntg_widget.h"
#include "core/widget/ntg_widget_vec.h"
#include "base/ntg_xy.h"
#include "base/object/ntg_object.h"

#define NTG_SCENE_MAX_IT_AUTO 20

struct ntg_scene_init_opts
{
    ntg_fcs_scope_keys_opt init_scope_keys; /* 0 for auto */

    unsigned int max_it; /* 0 for auto */
};

static const struct ntg_scene_init_opts NTG_SCENE_INIT_OPTS_ZERO = {0};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_scene_hit_res
{
    struct ntg_widget_hit_res res;
};

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

struct ntg_scene_layer_node;

struct ntg_scene
{
    ntg_object _base;

    struct
    {
        void* data;
    } pub;

    struct
    {
        struct ntg_widget_vec roots;
        size_t tree_count, object_count; /* Cache */

        ntg_stage* stage;
        struct ntg_xy size;
        ntg_fcs_manager* fm;
        bool dirty;
    } ro;

    struct
    {
        unsigned int max_it;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_scene_init(ntg_scene* scene, const struct ntg_scene_init_opts* opts);

NTG_API int
ntg_scene_deinit(ntg_scene* scene);


NTG_API int
ntg_scene_mark_dirty(ntg_scene* scene);

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

NTG_API size_t
ntg_scene_collect_layers_by_z(ntg_scene* scene, ntg_widget** out_buff, size_t cap);

/* ------------------------------------------------------ */

NTG_API int
ntg_scene_hit_test(
        ntg_scene* scene,
        struct ntg_xy pos,
        struct ntg_scene_hit_res* out_res);

/* ------------------------------------------------------ */

NTG_API int
ntg_scene_add_root(ntg_scene* scene, ntg_widget* widget);

NTG_API int
ntg_scene_rm_root(ntg_scene* scene, ntg_widget* widget);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_scene_feed_key(ntg_scene* scene, nt_key key);

/* ------------------------------------------------------ */

NTG_API bool
ntg_scene_feed_mouse(ntg_scene* scene, nt_mouse mouse);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_scene_vtable
{
    struct ntg_object_vtable base;

    bool (*handle_key_fn)(ntg_scene* scene, nt_key key);
    bool (*handle_mouse_fn)(ntg_scene* scene, nt_mouse mouse);
};

NTG_API int
ntg_scene_init_inherit(
        ntg_scene* scene,
        const struct ntg_scene_vtable* vtable,
        const ntg_type* type,
        const struct ntg_scene_init_opts* opts);

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

NTG_API extern const struct ntg_scene_vtable NTG_SCENE_VTABLE;

NTG_API bool
ntg_scene_dispatch_key_fn(ntg_scene* scene, nt_key key);

NTG_API bool
ntg_scene_dispatch_mouse_fn(ntg_scene* scene, nt_mouse mouse);

NTG_API void
ntg_scene_deinit_fn(ntg_object* _scene);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg__scene_set_size(ntg_scene* scene, struct ntg_xy size);
bool ntg__scene_layout(ntg_scene* scene, sarena* arena);

void ntg__scene_clean(ntg_scene* scene);

void ntg__scene_set_stage(ntg_scene* scene, ntg_stage* stage);
void ntg__scene_on_stage_enter(ntg_scene* scene, ntg_stage* stage);
void ntg__scene_on_stage_leave(ntg_scene* scene, ntg_stage* stage);

void ntg__scene_add_widget_tree(ntg_scene* scene, ntg_widget* root);
void ntg__scene_rm_widget_tree(ntg_scene* scene, ntg_widget* root);

void ntg__scene_on_add_widget_tree(ntg_scene* scene, ntg_widget* root);
void ntg__scene_on_rm_widget_tree(ntg_scene* scene, ntg_widget* root);

#endif // NTG_SCENE_H
