#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "ntg_focus_scope.h"
#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

#define NTG_SCENE_MAX_IT_AUTO 20

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_attach_policy;

struct ntg_scene_hooks
{
    void (*on_key_fn)(ntg_scene* scene, struct nt_key_event key);
    void (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse);

    void (*on_root_chng_fn)(
            ntg_scene* scene,
            ntg_object* old_root,
            ntg_object* new_root);

    void (*on_size_chng_fn)(
            ntg_scene* scene,
            struct ntg_xy old_size,
            struct ntg_xy new_size);

    void (*on_stage_enter_fn)(ntg_scene* scene, ntg_stage* stage);
    void (*on_stage_leave_fn)(ntg_scene* scene, ntg_stage* stage);
    
    void (*on_object_register_fn)(ntg_scene* scene, ntg_object* object);
    void (*on_object_unregister_fn)(ntg_scene* scene, ntg_object* object);

    void (*on_layer_add_fn)(ntg_scene* scene, ntg_object* layer_root);
    void (*on_layer_rm_fn)(ntg_scene* scene, ntg_object* layer_root);
};

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

struct ntg_scene_layer_node;

struct ntg_scene
{
    const struct ntg_scene_vtable* __vtable;

    ntg_stage* _stage;
    ntg_object* _root;
    struct ntg_xy _size;
    ntg_focus_manager* _fm;

    unsigned int __max_it;

    bool _dirty;

    struct ntg_scene_hooks hooks;

    void* data;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_scene_init(
        ntg_scene* scene,
        const struct ntg_focus_scope_keybinds* init_scope_keybinds,
        unsigned int max_it,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API void
ntg_scene_deinit(ntg_scene* scene);

/* ------------------------------------------------------ */


NTG_API void
ntg_scene_deinit_void(void* _scene);

/* ------------------------------------------------------ */


NTG_API void
ntg_scene_mark_dirty(ntg_scene* scene);

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

NTG_API ntg_object*
ntg_scene_hit_test(
        ntg_scene* scene,
        struct ntg_xy pos,
        struct ntg_xy* out_object_pos,
        ntg_object_hit_result* out_hit,
        int* out_status);

/* ------------------------------------------------------ */


NTG_API size_t
ntg_scene_collect_layers_by_z(
        ntg_scene* scene,
        ntg_object** out_layers,
        size_t cap);

/* ------------------------------------------------------ */


NTG_API void
ntg_scene_set_root(ntg_scene* scene, ntg_object* root, int* out_status);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */


NTG_API bool
ntg_scene_feed_key(ntg_scene* scene, struct nt_key_event key);

/* ------------------------------------------------------ */


NTG_API bool
ntg_scene_feed_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_scene_vtable
{
    bool (*handle_key_fn)(ntg_scene* scene, struct nt_key_event key);
    bool (*handle_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse);
};


NTG_API void
ntg_scene_init_override(
        ntg_scene* scene,
        const struct ntg_scene_vtable* vtable,
        const struct ntg_focus_scope_keybinds* init_scope_keybinds,
        unsigned int max_it,
        int* out_status);


NTG_API bool
ntg_scene_dispatch_key_fn(ntg_scene* scene, struct nt_key_event key);


NTG_API bool
ntg_scene_dispatch_mouse_fn(ntg_scene* scene, struct nt_mouse_event mouse);

NTG_API extern const struct ntg_scene_vtable NTG_SCENE_VTABLE_DEFAULT;

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void _ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size, int* out_status);
bool _ntg_scene_layout(ntg_scene* scene, sarena* arena);

void _ntg_scene_clean(ntg_scene* scene);

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);
void _ntg_scene_on_stage_enter(ntg_scene* scene, ntg_stage* stage);
void _ntg_scene_on_stage_leave(ntg_scene* scene, ntg_stage* stage);

void _ntg_scene_add_object_tree(ntg_scene* scene, ntg_object* root);
void _ntg_scene_rm_object_tree(ntg_scene* scene, ntg_object* root);

void _ntg_scene_register(ntg_scene* scene, ntg_object* root);
void _ntg_scene_unregister(ntg_scene* scene, ntg_object* root);

void _ntg_scene_register_tree(ntg_scene* scene, ntg_object* root);
void _ntg_scene_unregister_tree(ntg_scene* scene, ntg_object* root);

#endif // NTG_SCENE_H
