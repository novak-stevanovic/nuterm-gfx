#ifndef NTG_FOCUS_MANAGER_H
#define NTG_FOCUS_MANAGER_H

#include "shared/ntg_shared.h"
#include "core/scene/ntg_scene.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_focus_manager
{
    ntg_scene* _scene;

    ntg_scene_scope_list* __scope_stack;

    ntg_object* __focused;
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
/* FOCUSED */
/* -------------------------------------------------------------------------- */

bool ntg_focus_manager_request_focus(ntg_focus_manager* fm, ntg_object* object);
const ntg_object* ntg_focus_manager_get_focused(ntg_focus_manager* fm);
ntg_object* ntg_focus_manager_get_focused_(ntg_focus_manager* fm);

/* -------------------------------------------------------------------------- */
/* SCOPE STACK */
/* -------------------------------------------------------------------------- */

void ntg_focus_manager_push_scope(
        ntg_focus_manager* fm,
        const struct ntg_scene_scope* scope);

void ntg_focus_manager_pop_scope(ntg_focus_manager* fm);

const struct ntg_scene_scope*
ntg_focus_manager_get_active_scope(const ntg_focus_manager* fm);

size_t ntg_focus_manager_get_scope_count(const ntg_focus_manager* fm);

void ntg_focus_manager_invalidate(ntg_focus_manager* fm, ntg_object* removed);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_focus_manager_on_key(ntg_focus_manager* fm, struct nt_key_event key);
bool ntg_focus_manager_on_mouse(ntg_focus_manager* fm, struct nt_mouse_event mouse);

#endif // NTG_FOCUS_MANAGER_H
