#ifndef NTG_FOCUS_SCOPE_H
#define NTG_FOCUS_SCOPE_H

#include "shared/ntg_typedef.h"

struct ntg_focus_scope
{
    ntg_widget* root;
    void (*free_fn)(void* data);
    bool (*on_key_fn)(struct ntg_focus_scope* scope, struct nt_key_event key);
    bool (*on_mouse_fn)(struct ntg_focus_scope* scope, struct nt_mouse_event mouse);
    void* data;

    ntg_scene* scene; // set by the scene when pushing
};

bool ntg_focus_scope_dispatch_key(
        struct ntg_focus_scope* scope,
        struct nt_key_event key);

bool ntg_focus_scope_dispatch_mouse_def(
        struct ntg_focus_scope* scope,
        struct nt_mouse_event mouse);

bool ntg_focus_scope_dispatch_mouse_focus(
        struct ntg_focus_scope* scope,
        struct nt_mouse_event mouse);

#endif // NTG_FOCUS_SOPE_H
