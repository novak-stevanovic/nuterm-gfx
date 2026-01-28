#include <assert.h>
#include "ntg.h"
#include <stdlib.h>

GENC_SIMPLE_LIST_GENERATE(ntg_scene_focus_list, struct ntg_focus_scope_data);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_focus_scope_push(ntg_scene* scene, struct ntg_focus_scope_data data)
{
    assert(scene);
    assert(ntg_widget_is_descendant_eq(data.scope.root, scene->_root));

    ntg_scene_focus_list_pushf(scene->__focus_stack, data, NULL);
}

void ntg_scene_focus_scope_pop(ntg_scene* scene)
{
    assert(scene);
    
    ntg_scene_focus_list_popf(scene->__focus_stack, NULL);
}

bool ntg_scene_request_focus(ntg_scene* scene, ntg_widget* widget)
{
    assert(scene);

    struct ntg_focus_scope_data* head_data = scene->__focus_stack->head->data;

    if(!ntg_widget_is_descendant_eq(widget, head_data->scope.root))
        return false;

    head_data->focused = widget;

    return true;
}

ntg_widget* ntg_scene_get_focused(ntg_scene* scene)
{
    assert(scene);

    struct ntg_focus_scope_data* head_data = scene->__focus_stack->head->data;

    return head_data->focused;
}

bool ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena)
{
    assert(scene != NULL);

    struct ntg_xy old_size = scene->_size;
    scene->_size = size;

    bool root_change = scene->__root_change;
    scene->__root_change = false;

    bool recompose = scene->__layout_fn(scene, size, arena);

    return (!ntg_xy_are_equal(old_size, size) || root_change | recompose);
}

void ntg_scene_set_root(ntg_scene* scene, ntg_widget* root)
{
    assert(scene != NULL);
    if(scene->_root == root) return;
    if(root != NULL) assert(ntg_widget_get_parent(root) == NULL);

    ntg_widget* old_root = scene->_root;

    if(old_root != NULL)
    {
        ntg_object* old_root_gr = ntg_widget_get_group_root_(old_root);
        _ntg_object_detach_scene(old_root_gr);
    }

    if(root != NULL)
    {
        ntg_object* root_gr = ntg_widget_get_group_root_(root);
        _ntg_object_attach_scene(root_gr, scene);
    }

    scene->_root = root;

    struct ntg_event_scene_rootchng_data data = {
        .old = old_root,
        .new = root
    };

    scene->__root_change = true;

    ntg_entity_raise_event_((ntg_entity*)scene, NTG_EVENT_SCENE_ROOTCHNG, &data);
}

ntg_widget* ntg_scene_hit_test(ntg_scene* scene, struct ntg_xy pos)
{
    assert(scene);
    if(!scene->_root) return NULL;

    ntg_widget* root = scene->_root;
    
    struct ntg_xy root_start, root_end;
    root_start = ntg_widget_get_pos_abs(root);
    root_end = ntg_xy_add(root_start, ntg_widget_get_size(root));
    
    if(!(ntg_xy_is_in_rectagle(pos, root_start, root_end))) return NULL;

    ntg_widget* it_widget = root;
    ntg_widget** it_children;
    size_t it_children_size;
    ntg_widget* it_child;
    struct ntg_xy it_child_start, it_child_end;
    size_t i;
    bool loop = true;
    while(loop)
    {
        loop = false;
        it_children_size = ntg_widget_get_children(it_widget, NULL);
        if(it_children_size == 0) continue;
        it_children = malloc(sizeof(void*) * it_children_size);
        assert(it_children);
        ntg_widget_get_children_by_z(it_widget, it_children);
        for(i = it_children_size - 1; i >= 0; i--)
        {
            it_child = it_children[i];
            it_child_start = ntg_widget_get_pos_abs(it_child);
            it_child_end = ntg_xy_add(it_child_start, ntg_widget_get_size(it_child));
            if(ntg_xy_is_in_rectagle(pos, it_child_start, it_child_end))
            {
                it_widget = it_child;
                loop = true;
                break;
            }
        }
        free(it_children);
    }

    return it_widget;
}

bool ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key)
{
    assert(scene);

    return false;
}

bool ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    assert(scene);

    return false;
}

void ntg_scene_set_on_key_fn(ntg_scene* scene,
        bool (*on_key_fn)(ntg_scene* scene, struct nt_key_event key))
{
    assert(scene);

    scene->__on_key_fn = on_key_fn;
}

bool ntg_scene_on_key(ntg_scene* scene, struct nt_key_event key)
{
    assert(scene);

    if(scene->__on_key_fn)
        return scene->__on_key_fn(scene, key);
    else
        return false;
}

void ntg_scene_set_on_mouse_fn(ntg_scene* scene,
        bool (*on_mouse_fn)(ntg_scene* scene, struct nt_mouse_event mouse))
{
    assert(scene);

    assert(scene);

    scene->__on_mouse_fn = on_mouse_fn;
}

bool ntg_scene_on_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    assert(scene);

    if(scene->__on_mouse_fn)
        return scene->__on_mouse_fn(scene, mouse);
    else
        return false;
}

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_scene* scene)
{
    scene->_stage = NULL;

    scene->_size = ntg_xy(0, 0);
    scene->_root = NULL;
    scene->__layout_fn = NULL;

    scene->__on_key_fn = ntg_scene_dispatch_key;
    scene->__on_mouse_fn = ntg_scene_dispatch_mouse;
    scene->__focus_stack = NULL;

    scene->data = NULL;
}

void ntg_scene_init(ntg_scene* scene, ntg_scene_layout_fn layout_fn)
{
    assert(scene != NULL);
    assert(layout_fn != NULL);

    init_default(scene);

    scene->__layout_fn = layout_fn;
    scene->__focus_stack = malloc(sizeof(ntg_scene_focus_list));
    assert(scene->__focus_stack);
    ntg_scene_focus_list_init(scene->__focus_stack, NULL);
}

void ntg_scene_deinit(ntg_scene* scene)
{
    if(scene->_stage)
        ntg_stage_set_scene(scene->_stage, NULL);

    if(scene->_root)
    {
        ntg_object* root_gr = ntg_widget_get_group_root_(scene->_root);
        _ntg_object_detach_scene(root_gr);
    }

    size_t i;
    struct ntg_scene_focus_list_node* it = scene->__focus_stack->head;
    struct ntg_focus_scope_data it_data;
    for(i = 0; i < scene->__focus_stack->size; i++)
    {
        it_data = *(it->data);
        it_data.scope.free_fn(it_data.scope.data);
        it = it->next;
    }
    ntg_scene_focus_list_deinit(scene->__focus_stack, NULL);
    free(scene->__focus_stack);
    scene->__focus_stack = NULL;

    init_default(scene);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage)
{
    assert(scene != NULL);

    scene->_stage = stage;
}

void _ntg_scene_object_register(ntg_scene* scene, ntg_object* object)
{
    assert(scene);
    assert(object);

    _ntg_object_set_scene(object, scene);

    struct ntg_event_scene_objadd_data data = { .object = object };
    ntg_entity_raise_event_((ntg_entity*)scene, NTG_EVENT_SCENE_OBJADD, &data);
}

void _ntg_scene_object_unregister(ntg_scene* scene, ntg_object* object)
{
    assert(scene);
    assert(object);

    _ntg_object_set_scene(object, NULL);

    struct ntg_event_scene_objrm_data data = { .object = object };
    ntg_entity_raise_event_((ntg_entity*)scene, NTG_EVENT_SCENE_OBJRM, &data);
}
