#ifndef NTG_FOCUS_MANAGER_H
#define NTG_FOCUS_MANAGER_H

#include "shared/ntg_shared.h"
#include "core/scene/ntg_scene.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

enum ntg_focus_scope_input_mode
{
    NTG_FOCUS_SCOPE_INPUT_MODELESS,
    NTG_FOCUS_SCOPE_INPUT_MODAL
};

enum ntg_focus_scope_in_click_mode
{
    NTG_FOCUS_SCOPE_IN_CLICK_CLR_FIRST,
    NTG_FOCUS_SCOPE_IN_CLICK_KEEP,
    NTG_FOCUS_SCOPE_IN_CLICK_CLR,
    NTG_FOCUS_SCOPE_IN_CLICK_REFCS
};

enum ntg_focus_scope_out_click_mode
{
    NTG_FOCUS_SCOPE_OUT_CLICK_CLR,
    NTG_FOCUS_SCOPE_OUT_CLICK_KEEP
};

// Forbids pushing new scopes onto the stack
enum ntg_focus_scope_block_mode
{
    NTG_FOCUS_SCOPE_BLOCK_FALSE,
    NTG_FOCUS_SCOPE_BLOCK_TRUE
};

struct ntg_input_ctx
{
    ntg_focus_manager* fm;
    ntg_object* scope_root;
};

struct ntg_focus_scope
{
    ntg_object* root;

    ntg_focus_scope_input_mode input_mode;
    ntg_focus_scope_out_click_mode out_click_mode;
    ntg_focus_scope_in_click_mode in_click_mode;
    ntg_focus_scope_block_mode block_mode;

    bool (*on_key_fn)(
            void* data,
            const struct ntg_input_ctx* ctx,
            struct nt_key_event key);
    void* data;
};

struct ntg_focus_manager
{
    ntg_scene* _scene;

    ntg_object* _focused;

    ntg_focus_scope_list* __scope_stack;
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
        const struct ntg_focus_scope* scope);

void ntg_focus_manager_pop_scope(ntg_focus_manager* fm);

const struct ntg_focus_scope*
ntg_focus_manager_get_active_scope(const ntg_focus_manager* fm);

void ntg_focus_manager_invalidate(ntg_focus_manager* fm, ntg_object* removed);

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_focus_manager_on_key(ntg_focus_manager* fm, struct nt_key_event key);
bool ntg_focus_manager_on_mouse(ntg_focus_manager* fm, struct nt_mouse_event mouse);

#endif // NTG_FOCUS_MANAGER_H
