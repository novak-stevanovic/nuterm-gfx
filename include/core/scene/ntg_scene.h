#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "ntg_fcs_scope.h"
#include "shared/ntg_shared.h"
#include "core/object/ntg_object.h"
#include "core/object/ntg_objptr_vec.h"
#include "base/ntg_xy.h"

#define NTG_SCENE_MAX_IT_AUTO 20

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_attach_policy;

struct ntg_scene_hit_res
{
    struct ntg_object_hit_res res;
};

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

struct ntg_scene_layer_node;

struct ntg_scene
{
    struct
    {
        void* data;
    } pub;

    struct
    {
        struct ntg_objptr_vec roots;
        size_t tree_count, object_count; // Cached

        ntg_stage* stage;
        struct ntg_xy size;
        ntg_fcs_manager* fm;

        bool dirty;

        ntg_event_delegate event_dlgt;
    } ro;

    struct
    {
        const struct ntg_scene_vtable* vtable;
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
ntg_scene_init(
        ntg_scene* scene,
        const struct ntg_fcs_scope_keys* init_scope_keys,
        unsigned int max_it);

NTG_API int
ntg_scene_deinit(ntg_scene* scene);

NTG_API void
ntg_scene_deinit_void(void* _scene);

NTG_API int
ntg_scene_mark_dirty(ntg_scene* scene);

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

NTG_API size_t
ntg_scene_collect_layers_by_z(ntg_scene* scene, ntg_object** out_buff, size_t cap);

/* ------------------------------------------------------ */

NTG_API int
ntg_scene_hit_test(
        ntg_scene* scene,
        struct ntg_xy pos,
        struct ntg_scene_hit_res* out_res);

/* ------------------------------------------------------ */

NTG_API int
ntg_scene_add_root(ntg_scene* scene, ntg_object* object);

NTG_API int
ntg_scene_rm_root(ntg_scene* scene, ntg_object* object);

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
    bool (*handle_key_fn)(ntg_scene* scene, nt_key key);
    bool (*handle_mouse_fn)(ntg_scene* scene, nt_mouse mouse);
};

NTG_API int
ntg_scene_init_inherit(
        ntg_scene* scene,
        const struct ntg_scene_vtable* vtable,
        const struct ntg_fcs_scope_keys* init_scope_keys,
        unsigned int max_it);

NTG_API bool
ntg_scene_dispatch_key_fn(ntg_scene* scene, nt_key key);

NTG_API bool
ntg_scene_dispatch_mouse_fn(ntg_scene* scene, nt_mouse mouse);

NTG_API extern const struct ntg_scene_vtable NTG_SCENE_VTABLE_DEFAULT;

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

void ntg__scene_add_object_tree(ntg_scene* scene, ntg_object* root);
void ntg__scene_rm_object_tree(ntg_scene* scene, ntg_object* root);

void ntg__scene_on_add_object_tree(ntg_scene* scene, ntg_object* root);
void ntg__scene_on_rm_object_tree(ntg_scene* scene, ntg_object* root);

#endif // NTG_SCENE_H
