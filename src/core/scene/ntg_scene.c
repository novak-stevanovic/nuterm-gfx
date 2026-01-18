#include <assert.h>
#include "core/object/ntg_object.h"
#include "ntg.h"

static void on_object_register(ntg_scene* scene, ntg_object* object);
static void on_object_unregister(ntg_scene* scene, ntg_object* object);
static void on_object_register_fn(ntg_object* object, void* _scene);
static void on_object_unregister_fn(ntg_object* object, void* _scene);
static void object_observe_fn(ntg_entity* scene, struct ntg_event event);

NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(on_object_register_fn_tree, on_object_register_fn)
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(on_object_unregister_fn_tree, on_object_unregister_fn)

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_scene_layout(ntg_scene* scene, struct ntg_xy size, sarena* arena)
{
    assert(scene != NULL);

    scene->__layout_fn(scene, size, arena);
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
        _ntg_object_root_set_scene(old_root_gr, NULL);
    }

    if(root != NULL)
    {
        ntg_object* root_gr = ntg_widget_get_group_root_(root);
        on_object_register_fn_tree(root_gr, scene);
        _ntg_object_root_set_scene(root_gr, scene);
    }

    scene->_root = root;

    struct ntg_event_scene_rootchng_data data = {
        .old = old_root,
        .new = root
    };

    ntg_entity_raise_event((ntg_entity*)scene, NULL, NTG_EVENT_SCENE_ROOTCHNG, &data);
    ntg_entity_raise_event((ntg_entity*)scene, NULL, NTG_EVENT_SCENE_DIFF, NULL);
}

bool ntg_scene_feed_event(ntg_scene* scene,
        struct nt_event event, ntg_loop_ctx* ctx)
{
    assert(scene != NULL);

    return false;
    // TODO

    // return consumed;
}

void ntg_scene_set_event_fn(ntg_scene* scene, ntg_scene_event_fn fn)
{
    assert(scene != NULL);

    scene->__event_fn = fn;
}

void ntg_scene_set_event_mode(ntg_scene* scene, ntg_scene_event_mode mode)
{
    assert(scene != NULL);

    scene->__event_mode = mode;
}

/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */

static void init_default_values(ntg_scene* scene)
{
    scene->_stage = NULL;

    scene->_root = NULL;
    scene->__layout_fn = NULL;

    scene->__event_fn = NULL;
    scene->__event_mode = NTG_SCENE_EVENT_DISPATCH_FIRST;

    scene->data = NULL;
}

void _ntg_scene_init(ntg_scene* scene, ntg_scene_layout_fn layout_fn)
{
    assert(scene != NULL);
    assert(layout_fn != NULL);

    init_default_values(scene);

    scene->__layout_fn = layout_fn;
}

void _ntg_scene_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_scene* scene = (ntg_scene*)entity;
    init_default_values(scene);
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
    ntg_entity_observe((ntg_entity*)scene, (ntg_entity*)object, object_observe_fn);

    struct ntg_event_scene_objadd_data data = { .object = object };
    ntg_entity_raise_event((ntg_entity*)scene, NULL, NTG_EVENT_SCENE_OBJADD, &data);

    ntg_entity_raise_event((ntg_entity*)scene, NULL, NTG_EVENT_SCENE_DIFF, NULL);
}

static void on_object_unregister(ntg_scene* scene, ntg_object* object)
{
    ntg_entity_stop_observing((ntg_entity*)scene, (ntg_entity*)object, object_observe_fn);

    struct ntg_event_scene_objrm_data data = { .object = object };
    ntg_entity_raise_event((ntg_entity*)scene, NULL, NTG_EVENT_SCENE_OBJRM, &data);

    ntg_entity_raise_event((ntg_entity*)scene, NULL, NTG_EVENT_SCENE_DIFF, NULL);
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
        on_object_unregister_fn_tree(data->child, _scene);
    }
    else if(event.type == NTG_EVENT_OBJECT_DIFF)
    {
        ntg_entity_raise_event(_scene, NULL, NTG_EVENT_SCENE_DIFF, NULL);
    }
    else if(event.type == NTG_EVENT_OBJECT_PADCHNG)
    {
        struct ntg_event_object_padchng* data = event.data;

        if(scene->_root == event.source)
        {
            if(data->old != NULL)
                on_object_unregister(scene, (ntg_object*)data->old);

            if(data->new != NULL)
                on_object_register(scene, (ntg_object*)data->new);
        }
    }
    else if(event.type == NTG_EVENT_OBJECT_BORDCHNG)
    {
        struct ntg_event_object_bordchng* data = event.data;

        if(scene->_root == event.source)
        {
            if(data->old != NULL)
                on_object_unregister(scene, (ntg_object*)data->old);

            if(data->new != NULL)
                on_object_register(scene, (ntg_object*)data->new);
        }
    }
}
