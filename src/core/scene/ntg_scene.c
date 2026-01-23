#include <assert.h>
#include "core/object/ntg_object.h"
#include "ntg.h"

static void on_object_register(ntg_scene* scene, ntg_object* object);

static void on_object_unregister(ntg_scene* scene, ntg_object* object);

static void on_object_register_fn(ntg_object* object, void* _scene);

static void on_object_unregister_fn(ntg_object* object, void* _scene);

static void object_observe_fn(ntg_entity* scene, struct ntg_event event);

NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(on_object_register_fn_tree, on_object_register_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(on_object_unregister_fn_tree, on_object_unregister_fn);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

bool ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena)
{
    assert(scene != NULL);

    scene->_size = size;
    return scene->__layout_fn(scene, size, arena);
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
        on_object_unregister_fn_tree(old_root_gr, scene);
    }

    if(root != NULL)
    {
        ntg_object* root_gr = ntg_widget_get_group_root_(root);
        on_object_register_fn_tree(root_gr, scene);
    }

    scene->_root = root;

    struct ntg_event_scene_rootchng_data data = {
        .old = old_root,
        .new = root
    };

    ntg_entity_raise_event_((ntg_entity*)scene, NTG_EVENT_SCENE_ROOTCHNG, &data);
}

bool ntg_scene_feed_event(ntg_scene* scene, struct ntg_event event)
{
    assert(scene != NULL);

    ntg_log_log("Scene received event");

    return scene->__process_fn(scene, event);
}

bool ntg_scene_dispatch_def(ntg_scene* scene, struct ntg_event event)
{
    if(event.type == NTG_EVENT_LOOP_MOUSE)
    {
        bool dispatch = false;
        if(scene->__focus_ctx_stack.size == 0) dispatch = true;

        struct ntg_focus_ctx_list_node* head = scene->__focus_ctx_stack.head;
        struct ntg_focus_ctx fctx = *(head->data);

        dispatch |= (fctx.window_mode == NTG_FOCUS_CTX_WINDOW_MODELESS);

        struct ntg_event_loop_mouse_data* data = event.data;
        if(dispatch)
        {
            struct ntg_xy pos = ntg_xy(data->mouse.x, data->mouse.y);
            ntg_widget* widget = ntg_scene_get_widget_at(scene, pos);

            assert(widget != NULL);
            return ntg_widget_feed_event(widget, event);
        }
        else return false;
    }
    else if(event.type == NTG_EVENT_LOOP_KEY)
    {
        if(scene->__focus_ctx_stack.size == 0) return false;

        struct ntg_focus_ctx_list_node* head = scene->__focus_ctx_stack.head;
        struct ntg_focus_ctx fctx = *(head->data);

        if(head != NULL)
            return ntg_focus_mgr_feed_event(fctx.mgr, event);
        else
            return false;
    }
    else return false;
}

void ntg_scene_set_process_fn(ntg_scene* scene, ntg_scene_process_fn fn)
{
    assert(scene != NULL);

    scene->__process_fn = fn ? fn : ntg_scene_dispatch_def;
}

ntg_widget* ntg_scene_get_widget_at(ntg_scene* scene, struct ntg_xy pos)
{
    assert(scene != NULL);
    if(!scene->_root) return NULL;
    
    struct ntg_xy root_start, root_end;
    root_start = ntg_widget_get_pos_abs(scene->_root);
    root_end = ntg_xy_add(root_start, ntg_widget_get_size(scene->_root));
    
    if(!(ntg_xy_is_in_rectagle(root_start, root_end, pos))) return NULL;

    ntg_widget* it_widget = scene->_root;
    ntg_widget_vec it_children;
    ntg_widget* it_child;
    struct ntg_xy it_child_start, it_child_end;
    size_t i;
    bool loop = true;
    while(loop)
    {
        loop = false;
        ntg_widget_get_children(it_widget, &it_children);
        for(i = 0; i < it_children.size; i++)
        {
            it_child = it_children.data[i];
            it_child_start = ntg_widget_get_pos_abs(it_child);
            it_child_end = ntg_xy_add(it_child_start, ntg_widget_get_size(it_child));
            if(ntg_xy_is_in_rectagle(it_child_start, it_child_end, pos))
            {
                it_widget = it_child;
                loop = true;
                break;
            }
        }
        ntg_widget_vec_deinit(&it_children, NULL);
    }

    return it_widget;
}

void ntg_scene_focus_ctx_push(ntg_scene* scene, struct ntg_focus_ctx ctx)
{
    assert(scene != NULL);
    assert(ctx.mgr != NULL);

    ntg_focus_ctx_list_pushf(&scene->__focus_ctx_stack, ctx, NULL);
}

void ntg_scene_focus_ctx_pop(ntg_scene* scene)
{
    assert(scene != NULL);

    ntg_focus_ctx_list_popf(&scene->__focus_ctx_stack, NULL);
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

    scene->__process_fn = ntg_scene_dispatch_def;

    scene->data = NULL;
}

void ntg_scene_init(ntg_scene* scene, ntg_scene_layout_fn layout_fn)
{
    assert(scene != NULL);
    assert(layout_fn != NULL);

    init_default(scene);

    scene->__layout_fn = layout_fn;
    ntg_focus_ctx_list_init(&scene->__focus_ctx_stack, NULL);
}

void ntg_scene_deinit(ntg_scene* scene)
{
    if(scene->_stage)
        ntg_stage_set_scene(scene->_stage, NULL);

    if(scene->_root)
    {
        ntg_object* root_gr = ntg_widget_get_group_root_(scene->_root);
        on_object_unregister_fn_tree(root_gr, scene);
    }

    ntg_focus_ctx_list_deinit(&scene->__focus_ctx_stack, NULL);

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

static void on_object_register(ntg_scene* scene, ntg_object* object)
{
    _ntg_object_set_scene(object, scene);
    ntg_entity_observe((ntg_entity*)scene, (ntg_entity*)object, object_observe_fn);

    struct ntg_event_scene_objadd_data data = { .object = object };
    ntg_entity_raise_event_((ntg_entity*)scene, NTG_EVENT_SCENE_OBJADD, &data);
}

static void on_object_unregister(ntg_scene* scene, ntg_object* object)
{
    _ntg_object_set_scene(object, NULL);
    ntg_entity_stop_observing((ntg_entity*)scene, (ntg_entity*)object, object_observe_fn);

    struct ntg_event_scene_objrm_data data = { .object = object };
    ntg_entity_raise_event_((ntg_entity*)scene, NTG_EVENT_SCENE_OBJRM, &data);
}

static void on_object_register_fn(ntg_object* object, void* _scene)
{
    on_object_register((ntg_scene*)_scene, object);
}

static void on_object_unregister_fn(ntg_object* object, void* _scene)
{
    on_object_unregister((ntg_scene*)_scene, object);
}

static void object_observe_fn(ntg_entity* _scene, struct ntg_event event)
{
    ntg_scene* scene = (ntg_scene*)_scene;

    if(event.type == NTG_EVENT_OBJECT_CHLDADD)
    {
        struct ntg_event_object_chldadd_data* data = event.data;
        on_object_register_fn_tree(data->child, _scene);
    }
    else if(event.type == NTG_EVENT_OBJECT_CHLDRM)
    {
        struct ntg_event_object_chldrm_data* data = event.data;
        if(((ntg_widget*)data->child) == scene->_root) return;
        on_object_unregister_fn_tree(data->child, _scene);
    }
    else if(event.type == NTG_EVENT_WIDGET_PADCHNG)
    {
        struct ntg_event_widget_padchng_data* data = event.data;

        if(scene->_root == (ntg_widget*)event.source)
        {
            if(data->old != NULL)
                on_object_unregister(scene, (ntg_object*)data->old);

            if(data->new != NULL)
                on_object_register(scene, (ntg_object*)data->new);
        }
    }
    else if(event.type == NTG_EVENT_WIDGET_BORDCHNG)
    {
        struct ntg_event_widget_bordchng_data* data = event.data;

        if(scene->_root == (ntg_widget*)event.source)
        {
            if(data->old != NULL)
                on_object_unregister(scene, (ntg_object*)data->old);

            if(data->new != NULL)
                on_object_register(scene, (ntg_object*)data->new);
        }
    }
}
