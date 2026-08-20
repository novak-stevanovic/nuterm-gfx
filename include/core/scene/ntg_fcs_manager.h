#ifndef NTG_FCS_MANAGER_H
#define NTG_FCS_MANAGER_H

#include "ntg_fcs_scope.h"
#include "shared/ntg_shared.h"
#include "core/scene/ntg_scene.h"

struct ntg_fcs_scope;

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_fcs_manager
{
    struct
    {
        void* data;
    } pub;
    struct
    {
        ntg_scene* scene;
        ntg_object* focused;

        struct ntg_fcs_scope_keys default_keybinds;

        ntg_event_delegate event_dlgt;
    } ro;

    struct
    {
        ntg_fcs_scope_list* scope_stack;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* FOCUS */
/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_manager_request_focus(ntg_fcs_manager* fm, ntg_object* object);

/* ------------------------------------------------------ */
/* SCOPES */
/* ------------------------------------------------------ */

NTG_API int
ntg_fcs_manager_stack_push(ntg_fcs_manager* fm, ntg_fcs_scope* scope_own);

/* ------------------------------------------------------ */

NTG_API int
ntg_fcs_manager_stack_pop(ntg_fcs_manager* fm);

/* ------------------------------------------------------ */

NTG_API ntg_fcs_scope*
ntg_fcs_manager_stack_get_active(ntg_fcs_manager* fm);

/* ------------------------------------------------------ */

NTG_API size_t
ntg_fcs_manager_stack_get_size(const ntg_fcs_manager* fm);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_manager_feed_key(ntg_fcs_manager* fm, struct nt_key key);

/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_manager_feed_mouse(ntg_fcs_manager* fm, struct nt_mouse mouse);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg__fcs_manager_init(
        ntg_fcs_manager* fm,
        ntg_scene* scene,
        const struct ntg_fcs_scope_keys* init_scope_keybinds);

/* ------------------------------------------------------ */

void ntg__fcs_manager_deinit(ntg_fcs_manager* fm);

/* ------------------------------------------------------ */
/* INVALIDATE */
/* ------------------------------------------------------ */

void ntg__fcs_manager_on_scene_object_rm(ntg_fcs_manager* fm, ntg_object* removed);

#endif // NTG_FCS_MANAGER_H
