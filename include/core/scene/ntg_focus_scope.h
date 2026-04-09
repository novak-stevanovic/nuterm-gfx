#ifndef NTG_FOCUS_SCOPE_H
#define NTG_FOCUS_SCOPE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

enum ntg_focus_scope_input_mode
{
    NTG_FOCUS_SCOPE_INPUT_MODELESS,
    NTG_FOCUS_SCOPE_INPUT_MODAL
};

enum ntg_focus_scope_out_click_mode
{
    NTG_FOCUS_SCOPE_OUT_CLICK_KEEP,
    NTG_FOCUS_SCOPE_OUT_CLICK_CLR
};

// Forbids pushing new scopes onto the stack
enum ntg_focus_scope_block_mode
{
    NTG_FOCUS_SCOPE_BLOCK_FALSE,
    NTG_FOCUS_SCOPE_BLOCK_TRUE
};

struct ntg_focus_key_ctx
{
    ntg_focus_manager* fm;
    ntg_object* scope_root;
};

struct ntg_focus_mouse_ctx
{
    ntg_focus_manager* fm;
    ntg_object* scope_root;

    ntg_object* clicked;
    struct ntg_xy adj_pos; // position inside scope root space
};

struct ntg_focus_scope
{
    ntg_object* root;

    ntg_focus_scope_input_mode input_mode;
    ntg_focus_scope_out_click_mode out_click_mode;
    ntg_focus_scope_block_mode block_mode;

    bool (*on_key_fn)(
            void* data,
            const struct ntg_focus_key_ctx* ctx,
            struct nt_key_event key);

    bool (*on_mouse_fn)(
            void* data,
            const struct ntg_focus_mouse_ctx* ctx,
            struct nt_mouse_event mouse);
    void* data;
};

// Default implementation of `on_key_fn`. Dispatches event to focused element
bool ntg_focus_scope_dispatch_key(
        void* _,
        const struct ntg_focus_key_ctx* ctx,
        struct nt_key_event key);

/* Dispatches event to clicked element.
Doesn't take into account focused element */
bool ntg_focus_scope_dispatch_mouse_static(
        void* _,
        const struct ntg_focus_mouse_ctx* ctx,
        struct nt_mouse_event mouse);

/* Dispatches event to clicked element.
If no element is focused, the clicked element becomes focused.
If clicked element is not focused, the focused element loses focus. */
bool ntg_focus_scope_dispatch_mouse_dynamic(
        void* _,
        const struct ntg_focus_mouse_ctx* ctx,
        struct nt_mouse_event mouse);

#endif // NTG_FOCUS_SCOPE_H
