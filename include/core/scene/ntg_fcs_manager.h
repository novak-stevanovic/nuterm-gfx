#ifndef NTG_FCS_MANAGER_H
#define NTG_FCS_MANAGER_H

#include "ntg_fcs_scope.h"
#include "shared/ntg_shared.h"
#include "core/scene/ntg_scene.h"
#include "base/object/ntg_object.h"

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
    ntg_object _base;

    struct
    {
        void* data;
    } pub;
    struct
    {
        ntg_scene* scene;
        ntg_widget* focused;

        struct ntg_fcs_scope_keys default_keybinds;
    } ro;

    struct
    {
        struct ntg_fcs_scope_list* scope_stack;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* FOCUS */
/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_manager_request_focus(ntg_fcs_manager* fm, ntg_widget* widget);

/* ------------------------------------------------------ */
/* SCOPES */
/* ------------------------------------------------------ */

NTG_API int
ntg_fcs_manager_stack_push(ntg_fcs_manager* fm, const struct ntg_fcs_scope* scope_copy);

NTG_API int
ntg_fcs_manager_stack_pop(ntg_fcs_manager* fm);

NTG_API const struct ntg_fcs_scope*
ntg_fcs_manager_stack_get_active(ntg_fcs_manager* fm);

NTG_API size_t
ntg_fcs_manager_stack_get_size(const ntg_fcs_manager* fm);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_manager_feed_key(ntg_fcs_manager* fm, nt_key key);

/* ------------------------------------------------------ */

NTG_API bool
ntg_fcs_manager_feed_mouse(ntg_fcs_manager* fm, nt_mouse mouse);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

int ntg__fcs_manager_init(
        ntg_fcs_manager* fm,
        ntg_scene* scene,
        struct ntg_fcs_scope_keys init_keys);

void ntg__fcs_manager_deinit(ntg_fcs_manager* fm);

void ntg__fcs_manager_rm_wgt_from_scn(ntg_fcs_manager* fm, ntg_widget* removed);

#endif // NTG_FCS_MANAGER_H
