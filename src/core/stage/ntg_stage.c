#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void init_default(ntg_stage* stage);
static void init_default_drawing(ntg_stage* stage);

static void draw_object(ntg_stage* stage, ntg_object* object);
static bool draw_layer(ntg_stage* stage, ntg_object* root, sarena* arena);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_stage_init(ntg_stage* stage)
{
    return ntg_stage_init_inherit(stage, &NTG_STAGE_VTABLE_DEFAULT, &NTG_TYPE_STAGE);
}

int ntg_stage_deinit(ntg_stage* stage)
{
    if(!stage) return NTG_ERR_INV_ARG;

    if(stage->ro.in_loop)
    {
        ntg_loop_set_stage(NULL);
    }

    if(stage->ro.scene)
    {
        ntg_stage_set_scene(stage, NULL);
    }

    ntg_stage_draw_deinit(&stage->ro.drawing);

    ntg_entity_deinit(ntg_ent(stage));

    init_default(stage);

    return 0;
}

void ntg_stage_deinit_void(void* _stage)
{
    if(!_stage) return;

    ntg_stage_deinit(_stage);
}

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

bool ntg_stage_compose(ntg_stage* stage, sarena* arena)
{
    if(!stage || !arena) return false;

    ntg_log_log("COMPOSE");

    ntg_entity_event_raise(ntg_ent(stage), NTG_EVENT_STAGE_CMPSPRE, NULL);

    bool rval = false;
    struct ntg_xy size = stage->ro.size;

    int _status;
    if(!ntg_xy_are_eql(stage->ro.drawing.ro.size, size))
    {
        _status = ntg_stage_draw_set_size(&stage->ro.drawing, size);
        if(_status)
        {
            rval = true;
            goto done;
        }
    }

    if(stage->ro.scene && stage->ro.scene->ro.dirty)
    {
        if(!ntg__scene_layout(stage->ro.scene, arena))
            ntg__scene_clean(stage->ro.scene);
    }

    if(ntg_xy_is_zero_any(size)) goto done;
    if(!stage->ro.scene) goto done;

    size_t layer_count = ntg_scene_collect_layers_by_z(stage->ro.scene, NULL, SIZE_MAX);
    if(layer_count == 0) goto done;
    ntg_object** layers = sarena_malloc(arena, sizeof(ntg_object*) * layer_count);
    if(!layers)
    {
        rval = true;
        goto done;
    }

    ntg_scene_collect_layers_by_z(stage->ro.scene, layers, layer_count);

    // init_default_drawing(stage);

    size_t i;
    for(i = 0; i < layer_count; i++)
    {
        bool layer_redraw = draw_layer(stage, layers[i], arena);
        rval = rval || layer_redraw;
    }

done:
    ntg_entity_event_raise(ntg_ent(stage), NTG_EVENT_STAGE_CMPSPOST, NULL);

    return rval;
}

int ntg_stage_mark_dirty(ntg_stage* stage)
{
    if(!stage) return NTG_ERR_INV_ARG;

    stage->ro.dirty = true;

    return 0;
}

/* ------------------------------------------------------ */
/* SCENE */
/* ------------------------------------------------------ */

int ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    if(!stage)
        return NTG_ERR_INV_ARG;

    if(stage->ro.scene == scene) return 0;

    ntg_scene* old_scene = stage->ro.scene;
    ntg_stage* old_scene_stage = (old_scene ? old_scene->ro.stage : NULL);
    ntg_stage* old_stage = (scene ? scene->ro.stage : NULL);

    if(old_scene)
    {
        ntg__scene_set_stage(old_scene, NULL);

        /* Can only fail if size exceeds NTG_SIZE_MAX */
        ntg__scene_set_size(old_scene, ntg_xy(0, 0));
    }

    if(scene)
    {
        if(old_stage)
        {
            ntg_stage_set_scene(old_stage, NULL);
        }

        ntg__scene_set_stage(scene, stage);

        /* Can only fail if size exceeds NTG_SIZE_MAX */
        int status = ntg__scene_set_size(scene, stage->ro.size);
        if(status != 0)
            return status;
        ntg_scene_mark_dirty(scene);
    }

    stage->ro.scene = scene;

    struct ntg_event_stage_scnchg_dt event_dt = {
        .old_scene = old_scene,
        .new_scene = scene
    };
    ntg_entity_event_raise(ntg_ent(stage), NTG_EVENT_STAGE_SCNCHG, &event_dt);

    if(old_scene)
        ntg__scene_on_stage_leave(old_scene, old_scene_stage);

    if(scene)
        ntg__scene_on_stage_enter(scene, stage);

    return 0;
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_stage_feed_key(ntg_stage* stage, nt_key key)
{
    if(!stage) return false;
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    bool handled = false;

    if(ntg_stg_vtbl(stage) && ntg_stg_vtbl(stage)->handle_key_fn)
        handled = ntg_stg_vtbl(stage)->handle_key_fn(stage, key);

    struct ntg_event_stage_key_dt event_dt = { .key = key };
    ntg_entity_event_raise(ntg_ent(stage), NTG_EVENT_STAGE_KEY, &event_dt);

    return handled;
}

bool ntg_stage_feed_mouse(ntg_stage* stage, nt_mouse mouse)
{
    if(!stage) return false;
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return false;

    bool handled = false;

    if(ntg_stg_vtbl(stage) && ntg_stg_vtbl(stage)->handle_mouse_fn)
        handled = ntg_stg_vtbl(stage)->handle_mouse_fn(stage, mouse);

    struct ntg_event_stage_mouse_dt event_dt = { .mouse = mouse };
    ntg_entity_event_raise(ntg_ent(stage), NTG_EVENT_STAGE_MOUSE, &event_dt);

    return handled;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_stage_init_inherit(
        ntg_stage* stage,
        const struct ntg_stage_vtable* vtable,
        const ntg_type* type)
{
    if(!stage || !vtable || !type)
        return NTG_ERR_INV_ARG;

    if(!vtable->base.deinit_fn)
        return NTG_ERR_BAD_VTABLE;

    if(!ntg_type_instanceof(type, &NTG_TYPE_STAGE))
        return NTG_ERR_BAD_TYPE;

    init_default(stage);

    int status = ntg_entity_init_inherit(ntg_ent(stage), &vtable->base, type);
    switch(status)
    {
        case 0:
            break;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    int _status = ntg_stage_draw_init(&stage->ro.drawing);
    if(_status != 0)
    {
        ntg_entity_deinit(ntg_ent(stage));
        init_default(stage);
        return _status;
    }

    return 0;
}

bool ntg_stage_dispatch_key_fn(ntg_stage* stage, nt_key key)
{
    if(!stage) return false;

    if(stage->ro.scene)
        return ntg_scene_feed_key(stage->ro.scene, key);
    else
        return false;
}

bool ntg_stage_dispatch_mouse_fn(
        ntg_stage* stage,
        nt_mouse mouse)
{
    if(!stage) return false;

    if(stage->ro.scene)
        return ntg_scene_feed_mouse(stage->ro.scene, mouse);
    else
        return false;
}

void ntg_stage_deinit_fn(ntg_entity* _stage)
{
    ntg_stage_deinit(ntg_stg(_stage));
}

const struct ntg_stage_vtable NTG_STAGE_VTABLE_DEFAULT = {
    .base.deinit_fn = ntg_stage_deinit_fn,
    .handle_key_fn = ntg_stage_dispatch_key_fn,
    .handle_mouse_fn = ntg_stage_dispatch_mouse_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg__stage_set_size(ntg_stage* stage, struct ntg_xy size)
{
    if(!stage)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    if(ntg_xy_are_eql(stage->ro.size, size))
        return 0;

    struct ntg_xy old_size = stage->ro.size;

    stage->ro.size = size;
    ntg_stage_mark_dirty(stage);

    int _status;

    if(stage->ro.scene)
    {
        _status = ntg__scene_set_size(stage->ro.scene, size);
        if(_status != 0)
            return _status;
    }

    struct ntg_event_stage_szchg_dt event_dt = {
        .old_x = old_size.x,
        .old_y = old_size.y,
        .new_x = size.x,
        .new_y = size.y
    };
    ntg_entity_event_raise(ntg_ent(stage), NTG_EVENT_STAGE_SZCHG, &event_dt);

    return 0;
}

void ntg__stage_clean(ntg_stage* stage)
{
    if(!stage) return;

    stage->ro.dirty = false;

}

void ntg__stage_enter_loop(ntg_stage* stage)
{
    if(!stage) return;

    stage->ro.in_loop = true;
    ntg_stage_mark_dirty(stage);

    ntg_entity_event_raise(ntg_ent(stage), NTG_EVENT_STAGE_ENTER, NULL);

}

void ntg__stage_leave_loop(ntg_stage* stage)
{
    if(!stage) return;

    stage->ro.in_loop = false;

    ntg_entity_event_raise(ntg_ent(stage), NTG_EVENT_STAGE_LEAVE, NULL);

}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void init_default(ntg_stage* stage)
{
    if(!stage) return;

    (*stage) = (ntg_stage) {0};
}

static void init_default_drawing(ntg_stage* stage)
{
    if(!stage) return;

    struct ntg_xy size = stage->ro.drawing.ro.size;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_stage_draw_set(
                    &stage->ro.drawing,
                    ntg_cell_default(),
                    ntg_xy(j, i));
        }
    }
}

static void draw_object(ntg_stage* stage, ntg_object* object)
{
    if(!object) return;

    struct ntg_xy abs_pos;
    abs_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(object, ntg_dxy(0, 0)));

    struct ntg_xy draw_size = object->ro.drawing.ro.size;

    // If size is equal, use the drawing, even if last draw failed.
    if(ntg_xy_size_are_eql(object->ro.size, draw_size))
    {
        ntg_object_draw_place_(
                &object->ro.drawing,
                &stage->ro.drawing,
                abs_pos);
    }

    if(!(object->ro.dirty & NTG_OBJECT_DIRTY_DRAW))
        ntg__object_clean(object, NTG__OBJECT_DIRTY_RENDER);
}

static int get_objects_in_draw_order_layer(
        ntg_object* root,
        ntg_object** buff,
        size_t* counter,
        sarena* arena)
{
    if(!root) return 0;

    buff[*counter] = root;
    (*counter)++;

    const struct ntg_objptr_vec* children = &root->ro.children;

    if(children->size == 0) return 0;

    // Init children_by_z
    ntg_object** children_by_z = sarena_malloc(arena, sizeof(ntg_object*) * children->size);
    if(!children_by_z) return NTG_ERR_ALLOC_FAIL;
    memcpy(children_by_z, children->data, sizeof(ntg_object*) * children->size);

    // Sort children_by_z
    ntg_object_sort_by_z(children_by_z, children->size);

    size_t i;
    int status;
    for(i = 0; i < children->size; i++)
    {
        status = get_objects_in_draw_order_layer(children_by_z[i], buff, counter, arena);
        if(status) return NTG_ERR_ALLOC_FAIL;
    }

    return 0;
}

static bool draw_layer(ntg_stage* stage, ntg_object* root, sarena* arena)
{
    if(!root) return false;

    // naive attempt
    const size_t init_cap = 100;

    ntg_object** buff = sarena_calloc(arena, sizeof(ntg_object*) * init_cap);
    if(!buff) return true;

    size_t tree_size = ntg_object_tree_collect_pre(root, buff, init_cap);
    if(tree_size == 0) return false;

    if(tree_size > init_cap)
    {
        buff = sarena_malloc(arena, sizeof(ntg_object*) * tree_size);
        if(!buff) return true;
    }

    size_t _counter = 0;
    get_objects_in_draw_order_layer(root, buff, &_counter, arena);

    size_t i;
    for(i = 0; i < tree_size; i++)
        draw_object(stage, buff[i]);

    return false;
}
