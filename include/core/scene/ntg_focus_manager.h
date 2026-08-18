#ifndef NTG_FOCUS_MANAGER_H
#define NTG_FOCUS_MANAGER_H

#include "ntg_focus_scope.h"
#include "shared/ntg_shared.h"
#include "core/scene/ntg_scene.h"

struct ntg_focus_scope;

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_focus_manager_hooks
{
    void (*on_focused_chng_fn)(
            ntg_focus_manager* fm,
            ntg_object* old_focused,
            ntg_object* new_focused);

    void (*on_scope_push_fn)(ntg_focus_manager* fm, ntg_focus_scope* scope);
    void (*on_scope_pop_fn)(ntg_focus_manager* fm, ntg_focus_scope* scope);
};

struct ntg_focus_manager
{
    ntg_scene* _scene;

    ntg_object* _focused;

    ntg_focus_scope_list* __scope_stack;

    struct ntg_focus_manager_hooks hooks;

    struct ntg_focus_scope_keybinds _default_keybinds;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* FOCUS */
/* ------------------------------------------------------ */


NTG_API bool
ntg_focus_manager_request_focus(ntg_focus_manager* fm, ntg_object* object);

/* ------------------------------------------------------ */
/* SCOPES */
/* ------------------------------------------------------ */

NTG_API int
ntg_focus_manager_stack_push(
        ntg_focus_manager* fm,
        const ntg_focus_scope* scope,
        ntg_focus_scope** out_scope);

/* ------------------------------------------------------ */

NTG_API void
ntg_focus_manager_stack_pop(ntg_focus_manager* fm);

/* ------------------------------------------------------ */

NTG_API ntg_focus_scope*
ntg_focus_manager_stack_get_active(ntg_focus_manager* fm);

/* ------------------------------------------------------ */

NTG_API size_t
ntg_focus_manager_stack_get_size(const ntg_focus_manager* fm);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_focus_manager_feed_key(ntg_focus_manager* fm, struct nt_key_event key);

/* ------------------------------------------------------ */

NTG_API bool
ntg_focus_manager_feed_mouse(ntg_focus_manager* fm, struct nt_mouse_event mouse);

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int _ntg_focus_manager_init(
        ntg_focus_manager* fm,
        ntg_scene* scene,
        const struct ntg_focus_scope_keybinds* init_scope_keybinds);

/* ------------------------------------------------------ */

void _ntg_focus_manager_deinit(ntg_focus_manager* fm);

/* ------------------------------------------------------ */
/* INVALIDATE */
/* ------------------------------------------------------ */

void _ntg_focus_manager_on_scene_object_rm(ntg_focus_manager* fm, ntg_object* removed);

#endif // NTG_FOCUS_MANAGER_H
