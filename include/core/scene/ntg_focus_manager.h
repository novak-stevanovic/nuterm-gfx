#ifndef NTG_FOCUS_MANAGER_H
#define NTG_FOCUS_MANAGER_H

#include "shared/ntg_shared.h"
#include "core/scene/ntg_scene.h"

struct ntg_focus_scope;

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_focus_manager_hooks
{
    void (*on_focused_chng_fn)(
            ntg_focus_manager* fm,
            ntg_object* old_focused,
            ntg_object* new_focused);

    void (*on_scope_push_fn)(ntg_focus_manager* fm, const struct ntg_focus_scope* scope);
    void (*on_scope_pop_fn)(ntg_focus_manager* fm, const struct ntg_focus_scope* scope);
};

struct ntg_focus_manager
{
    ntg_scene* _scene;

    ntg_object* _focused;

    ntg_focus_scope_list* __scope_stack;

    struct ntg_focus_manager_hooks hooks;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* FOCUS */
/* ------------------------------------------------------ */

/* Requests focus for `object`, or clears focus when `object` is `NULL`. A
 * non-`NULL` object must lie within the active rooted scope. Focus and unfocus
 * hooks are notified, but their return values are ignored.
 *
 * RETURN VALUE:
 * `true` when the scope permits the requested state; otherwise `false`. */
NTG_API bool
ntg_focus_manager_request_focus(ntg_focus_manager* fm, ntg_object* object);

// NTG_API ntg_object*
// ntg_focus_manager_find(ntg_focus_manager* fm, int diff);

/* ------------------------------------------------------ */
/* SCOPES */
/* ------------------------------------------------------ */

/* Pushes a focus scope unless the current scope blocks pushes. A rooted scope
 * must belong to one of the scene layers; focus is cleared after a successful
 * push.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `fm` or `scope` is `NULL`.
 * - `NTG_ERR_SCENE_EMPTY`: a rooted scope is requested while the scene has no
 *   layers.
 * - `NTG_ERR_SCOPE_NOT_IN_SCENE`: the scope root is not inside any scene layer.
 * - `NTG_ERR_ALLOC_FAIL`: the scope stack cannot grow.
 * - `NTG_ERR_UNEXPECTED`: the scope-stack operation fails unexpectedly. */
NTG_API void
ntg_focus_manager_push_scope(
        ntg_focus_manager* fm,
        const struct ntg_focus_scope* scope,
        int* out_status);

/* ------------------------------------------------------ */

/* Pops the active scope, restores the previous scope and its last focused
 * object when valid, and always keeps the default scope. A `NULL` manager is
 * ignored. */
NTG_API void
ntg_focus_manager_pop_scope(ntg_focus_manager* fm);

/* ------------------------------------------------------ */

/* Returns the scope at the top of an initialized focus-manager stack.
 *
 * RETURN VALUE:
 * The active scope, or `NULL` when `fm` is `NULL` or the stack is empty. */
NTG_API const struct ntg_focus_scope*
ntg_focus_manager_get_active_scope(const ntg_focus_manager* fm);

/* ------------------------------------------------------ */

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

/* Passes a key event to the active scope handler.
 *
 * RETURN VALUE:
 * `true` when the active scope handles the event; otherwise `false`. */
NTG_API bool
ntg_focus_manager_feed_key(ntg_focus_manager* fm, struct nt_key_event key);

/* ------------------------------------------------------ */

/* Hit-tests the scene, applies active-scope modal and outside-click rules, then
 * passes the mouse event to the active scope handler or eligible outside
 * object.
 *
 * RETURN VALUE:
 * `true` when the event is handled; otherwise `false`. */
NTG_API bool
ntg_focus_manager_feed_mouse(ntg_focus_manager* fm, struct nt_mouse_event mouse);

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes scene focus state and pushes the built-in default focus scope.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `fm` or `scene` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: scope-stack storage cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: default-scope creation fails unexpectedly. */
NTG_API void
_ntg_focus_manager_init(ntg_focus_manager* fm, ntg_scene* scene, int* out_status);

/* ------------------------------------------------------ */

/* Clears focus, releases the scope stack, and resets the manager. Passing
 * `NULL` has no effect. */
NTG_API void
_ntg_focus_manager_deinit(ntg_focus_manager* fm);

/* ------------------------------------------------------ */
/* INVALIDATE */
/* ------------------------------------------------------ */

/* Marks each rooted scope invalid when `removed` is equal to or lies below that
 * scope root, then synchronizes the stack and restores or clears focus as
 * required. A `NULL` manager is ignored. */
NTG_API void
_ntg_focus_manager_invalidate(ntg_focus_manager* fm, ntg_object* removed);

#endif // NTG_FOCUS_MANAGER_H
