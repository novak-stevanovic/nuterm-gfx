#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_attach_policy;

// Should be taken into account by the scene's `on_key_fn` and `on_mouse_fn`
enum ntg_scene_scope_input_mode
{
    NTG_SCENE_SCOPE_INPUT_MODELESS,
    NTG_SCENE_SCOPE_INPUT_MODAL
};

// Should be taken into account by the scene's `on_key_fn` and `on_mouse_fn`
enum ntg_scene_scope_click_mode
{
    NTG_SCENE_SCOPE_CLICK_KEEP_FOCUS,
    NTG_SCENE_SCOPE_CLICK_CLEAR_FOCUS
};

// Forbids pushing new scopes onto the stack
enum ntg_scene_scope_block_mode
{
    NTG_SCENE_SCOPE_BLOCK_FALSE,
    NTG_SCENE_SCOPE_BLOCK_TRUE
};

struct ntg_focus_ctx
{
    ntg_scene* scene;
    ntg_object* scope_root;
};

struct ntg_scene_scope
{
    ntg_object* root;
    void (*on_key_fn)(void* data, const struct ntg_focus_ctx* ctx);
    ntg_scene_scope_input_mode input_mode;
    ntg_scene_scope_click_mode click_mode;
    ntg_scene_scope_block_mode block_mode;
    void* data;
};

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

    ntg_focus_manager* __fm;

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

size_t ntg_scene_collect_layers_by_z(
        ntg_scene* scene,
        ntg_object** out_layers,
        size_t cap);
void ntg_scene_set_root(ntg_scene* scene, ntg_object* root);

bool ntg_scene_request_focus(ntg_scene* scene, ntg_object* object);
const ntg_object* ntg_scene_get_focused(const ntg_scene* scene);
ntg_object* ntg_scene_get_focused_(ntg_scene* scene);

void ntg_scene_scope_stack_push(ntg_scene* scene, const struct ntg_scene_scope* scope);
void ntg_scene_scope_stack_pop(ntg_scene* scene);
const struct ntg_scene_scope* ntg_scene_get_active_scope(const ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

// Default implementations. Dispatches events to focus manager
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

// Called internally by ntg_stage. Updates only the scene's state
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

void _ntg_scene_register(ntg_scene* scene, ntg_object* root);
void _ntg_scene_unregister(ntg_scene* scene, ntg_object* root);
void _ntg_scene_register_tree(ntg_scene* scene, ntg_object* root);
void _ntg_scene_unregister_tree(ntg_scene* scene, ntg_object* root);

#endif // NTG_SCENE_H
