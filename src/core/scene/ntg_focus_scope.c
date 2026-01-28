#include "ntg.h"
#include <assert.h>
#include <stdlib.h>

bool ntg_focus_scope_dispatch_key(
        struct ntg_focus_scope* scope,
        struct nt_key_event key)
{
    assert(scope);

    if(!scope->scene) return false;

    ntg_widget* focused = ntg_scene_get_focused(scope->scene);

    if(focused)
        return ntg_widget_on_key(focused, key);
    else
        return false;
}

bool ntg_focus_scope_dispatch_mouse_def(
        struct ntg_focus_scope* scope,
        struct nt_mouse_event mouse)
{
    assert(scope);

    if(!scope->scene) return false;

    struct ntg_xy pos = ntg_xy(mouse.x, mouse.y);
    struct ntg_xy abs_pos = ntg_widget_map_to_scene_space(scope->root, pos);

    ntg_widget* hit = ntg_scene_hit_test(scope->scene, abs_pos);
    assert(hit && ((hit == scope->root) ||
    ntg_widget_is_ancestor(hit, scope->root)));
    
    struct ntg_xy hit_pos = ntg_widget_map_to_descendant_space(
            scope->root, hit, pos);

    mouse.x = hit_pos.x;
    mouse.y = hit_pos.y;

    return ntg_widget_on_mouse(hit, mouse);
}

bool ntg_focus_scope_dispatch_mouse_focus(
        struct ntg_focus_scope* scope,
        struct nt_mouse_event mouse)
{
    struct ntg_xy pos = ntg_xy(mouse.x, mouse.y);
    struct ntg_xy abs_pos = ntg_widget_map_to_scene_space(scope->root, pos);

    ntg_widget* hit = ntg_scene_hit_test(scope->scene, abs_pos);
    assert(hit && ((hit == scope->root) ||
    ntg_widget_is_ancestor(hit, scope->root)));

    ntg_widget* focused = ntg_scene_get_focused(scope->scene);
    
    struct ntg_xy hit_pos = ntg_widget_map_to_descendant_space(
            scope->root, hit, pos);

    mouse.x = hit_pos.x;
    mouse.y = hit_pos.y;

    if(hit != focused)
    {
        ntg_scene_request_focus(scope->scene, hit);
    }

    return ntg_widget_on_mouse(hit, mouse);
}
