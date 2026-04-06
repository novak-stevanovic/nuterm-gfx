#ifndef NTG_FOCUS_MANAGER_H
#define NTG_FOCUS_MANAGER_H

#include "shared/ntg_shared.h"
#include "core/scene/ntg_scene.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

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

struct ntg_focus_manager
{
    ntg_scene* _scene;

    ntg_object* _focused;

    ntg_scene_scope_list* __scope_stack;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

void ntg_focus_manager_init(ntg_focus_manager* fm, ntg_scene* scene);
void ntg_focus_manager_deinit(ntg_focus_manager* fm);
void ntg_focus_manager_deinit_(void* _fm);

/* -------------------------------------------------------------------------- */
/* GENERAL */
/* -------------------------------------------------------------------------- */

bool ntg_focus_manager_request_focus(ntg_focus_manager* fm, ntg_object* object);

void ntg_focus_manager_push_scope(
        ntg_focus_manager* fm,
        const struct ntg_scene_scope* scope);

void ntg_focus_manager_pop_scope(ntg_focus_manager* fm);

const struct ntg_scene_scope*
ntg_focus_manager_get_active_scope(const ntg_focus_manager* fm);

void ntg_focus_manager_invalidate(ntg_focus_manager* fm, ntg_object* removed);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_focus_manager_on_key(ntg_focus_manager* fm, struct nt_key_event key);
bool ntg_focus_manager_on_mouse(ntg_focus_manager* fm, struct nt_mouse_event mouse);

#endif // NTG_FOCUS_MANAGER_H
