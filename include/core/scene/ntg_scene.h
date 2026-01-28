#ifndef NTG_SCENE_H
#define NTG_SCENE_H

#include "core/entity/ntg_entity.h"
#include "shared/ntg_xy.h"
#include "core/scene/ntg_focus_scope.h"
#include "core/object/widget/ntg_widget.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

enum ntg_focus_scope_window_mode
{
    NTG_FOCUS_CTX_WINDOW_MODAL,
    NTG_FOCUS_CTX_WINDOW_MODELESS
};

enum ntg_focus_scope_push_mode
{
    NTG_FOCUS_CTX_PUSH_ALLOW,
    NTG_FOCUS_CTX_PUSH_FORBID
};

struct ntg_focus_scope_data
{
    struct ntg_focus_scope scope;
    ntg_widget* focused; // init focused on pushing
    ntg_focus_scope_window_mode window_mode;
    ntg_focus_scope_push_mode push_mode;
};

struct ntg_scene
{
    ntg_entity __base;

    struct
    {
        ntg_stage* _stage;
    };

    struct
    {
        struct ntg_xy _size;
        bool __root_change;
        ntg_widget* _root;
        ntg_scene_layout_fn __layout_fn;
    };

    struct
    {
        ntg_scene_focus_list* __focus_stack;
    };

    struct
    {
        bool (*__on_key_fn)(ntg_scene* scene, struct nt_key_event key);
        bool (*__on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse);
    };

    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_focus_scope_push(ntg_scene* scene, struct ntg_focus_scope_data data);
void ntg_scene_focus_scope_pop(ntg_scene* scene);
bool ntg_scene_request_focus(ntg_scene* scene, ntg_widget* widget);
ntg_widget* ntg_scene_get_focused(ntg_scene* scene);

bool ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena);
void ntg_scene_set_root(ntg_scene* scene, ntg_widget* root);

bool ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key);
bool ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

void ntg_scene_set_on_key_fn(ntg_scene* scene,
        bool (*on_key_fn)(ntg_scene* scene, struct nt_key_event key));
bool ntg_scene_on_key(ntg_scene* scene, struct nt_key_event key);

void ntg_scene_set_on_mouse_fn(ntg_scene* scene,
        bool (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse));
bool ntg_scene_on_mouse(ntg_scene* scene, struct nt_mouse_event mouse);

ntg_widget* ntg_scene_hit_test(ntg_scene* scene, struct ntg_xy pos);

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_scene_init(ntg_scene* scene, ntg_scene_layout_fn layout_fn);
void ntg_scene_deinit(ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

// Called internally by ntg_stage. Updates only the scene's state
void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage);

void _ntg_scene_object_register(ntg_scene* scene, ntg_object* object);
void _ntg_scene_object_unregister(ntg_scene* scene, ntg_object* object);

#endif // NTG_SCENE_H
