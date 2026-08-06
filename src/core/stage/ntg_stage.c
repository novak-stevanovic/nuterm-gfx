#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static const struct ntg_stage_vtable VTABLE_EMPTY = {0};

static void init_default(ntg_stage* stage);

static void get_objects_in_drawing_order_layer(ntg_object* root, ntg_object** buff);
static void get_objects_in_drawing_order_layer_internal(
        ntg_object* root,
        ntg_object** buff,
        size_t* counter);
static void draw_object(ntg_stage* stage, ntg_object* object);
static bool draw_layer(ntg_stage* stage, ntg_object* root, sarena* arena);

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

void ntg_stage_init(ntg_stage* stage, int* out_status)
{
    ntg_stage_init_override(stage, &NTG_STAGE_VTABLE_DEFAULT, out_status);
}

void ntg_stage_deinit(ntg_stage* stage)
{
    if(!stage) return;

    if(stage->_in_loop)
    {
        ntg_loop_set_stage(NULL, NULL);
    }

    if(stage->_scene)
    {
        ntg_stage_set_scene(stage, NULL, NULL);
    }

    ntg_stage_drawing_deinit(&stage->_drawing);

    init_default(stage);
}

void ntg_stage_deinit_void(void* _stage)
{
    if(!_stage) return;

    ntg_stage_deinit(_stage);
}

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene, int* out_status)
{
    ntg_init_status(out_status);

    if(!stage)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(stage->_scene == scene) return;

    ntg_scene* old_scene = stage->_scene;
    ntg_stage* old_scene_stage = (old_scene ? old_scene->_stage : NULL);
    ntg_stage* old_stage = (scene ? scene->_stage : NULL);

    if(old_scene)
    {
        _ntg_scene_set_stage(old_scene, NULL);

        _ntg_scene_set_size(old_scene, ntg_xy(0, 0));
    }

    if(scene)
    {
        if(old_stage)
        {
            ntg_stage_set_scene(old_stage, NULL, NULL);
        }

        _ntg_scene_set_stage(scene, stage);
        _ntg_scene_set_size(scene, stage->_size);
        ntg_scene_mark_dirty(scene);
    }

    stage->_scene = scene;

    if(stage->hooks.on_scene_chng_fn)
        stage->hooks.on_scene_chng_fn(stage, old_scene, scene);

    if(old_scene)
        _ntg_scene_on_stage_leave(old_scene, old_scene_stage);

    if(scene)
        _ntg_scene_on_stage_enter(scene, stage);
}

void ntg_stage_mark_dirty(ntg_stage* stage)
{
    if(!stage) return;

    stage->_dirty = true;
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_stage_feed_key(ntg_stage* stage, struct nt_key_event key)
{
    if(!stage) return false;

    bool handled = false;

    if(stage->__vtable && stage->__vtable->handle_key_fn)
        handled = stage->__vtable->handle_key_fn(stage, key);

    if(stage->hooks.on_key_fn)
        stage->hooks.on_key_fn(stage, key);

    return handled;
}

bool ntg_stage_feed_mouse(ntg_stage* stage, struct nt_mouse_event mouse)
{
    if(!stage) return false;

    bool handled = false;

    if(stage->__vtable && stage->__vtable->handle_mouse_fn)
        handled = stage->__vtable->handle_mouse_fn(stage, mouse);

    if(stage->hooks.on_mouse_fn)
        stage->hooks.on_mouse_fn(stage, mouse);

    return handled;
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_stage_init_override(
        ntg_stage* stage,
        const struct ntg_stage_vtable* vtable,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!stage)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    init_default(stage);
    stage->__vtable = (vtable ? vtable : &VTABLE_EMPTY);

    int _status;
    ntg_stage_drawing_init(&stage->_drawing, &_status);

    if(_status != 0)
    {
        init_default(stage);
        ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }
}

bool ntg_stage_dispatch_key_fn(ntg_stage* stage, struct nt_key_event key)
{
    if(!stage) return false;

    if(stage->_scene)
        return ntg_scene_feed_key(stage->_scene, key);
    else
        return false;
}

bool ntg_stage_dispatch_mouse_fn(
        ntg_stage* stage,
        struct nt_mouse_event mouse)
{
    if(!stage) return false;

    if(stage->_scene)
        return ntg_scene_feed_mouse(stage->_scene, mouse);
    else
        return false;
}

const struct ntg_stage_vtable NTG_STAGE_VTABLE_DEFAULT = {
    .handle_key_fn = ntg_stage_dispatch_key_fn,
    .handle_mouse_fn = ntg_stage_dispatch_mouse_fn
};

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

bool _ntg_stage_compose(ntg_stage* stage, sarena* arena, int* out_recompose)
{
    if(!stage || !arena) return false;

    struct ntg_xy size = stage->_size;

    int _status;
    if(!ntg_xy_are_eql(ntg_stage_drawing_get_size(&stage->_drawing), size))
    {
        struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
        ntg_stage_drawing_set_size(&stage->_drawing, size, size_cap, &_status);
        if(_status != 0)
            return true;
    }

    int _relayout = 0;
    if(stage->_scene && stage->_scene->_dirty)
    {
        _ntg_scene_layout(stage->_scene, arena, &_relayout);
        if(!_relayout)
            _ntg_scene_clean(stage->_scene);
    }

    if(ntg_xy_size_is_zero(size)) return false;

    size_t i, j;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            ntg_stage_drawing_set(
                    &stage->_drawing,
                    ntg_cell_default(),
                    ntg_xy(j, i));
        }
    }

    if(!stage->_scene) return false;
    if(!stage->_scene->_root) return false;

    size_t layer_count = ntg_scene_collect_layers_by_z(stage->_scene, NULL, 0);

    ntg_object** layers = sarena_calloc(arena, sizeof(ntg_object*) * layer_count);
    if(!layers)
        return true;

    ntg_scene_collect_layers_by_z(stage->_scene, layers, layer_count);

    bool rval = false;
    for(i = 0; i < layer_count; i++)
    {
        rval = rval ||  draw_layer(stage, layers[i], arena);
    }

    return rval;
}

void _ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size)
{
    if(!stage) return;

    if(ntg_xy_are_eql(stage->_size, size))
        return;

    struct ntg_xy old_size = stage->_size;

    stage->_size = size;
    ntg_stage_mark_dirty(stage);

    if(stage->_scene)
        _ntg_scene_set_size(stage->_scene, size);

    if(stage->hooks.on_size_chng_fn)
        stage->hooks.on_size_chng_fn(stage, old_size, size);
}

void _ntg_stage_clean(ntg_stage* stage)
{
    if(!stage) return;

    stage->_dirty = false;
}

void _ntg_stage_enter_loop(ntg_stage* stage)
{
    if(!stage) return;

    stage->_in_loop = true;
    ntg_stage_mark_dirty(stage);

    if(stage->hooks.on_loop_enter_fn)
        stage->hooks.on_loop_enter_fn(stage);
}

void _ntg_stage_leave_loop(ntg_stage* stage)
{
    if(!stage) return;

    stage->_in_loop = false;

    if(stage->hooks.on_loop_leave_fn)
        stage->hooks.on_loop_leave_fn(stage);
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_stage* stage)
{
    (*stage) = (ntg_stage) {0};
}

static void get_objects_in_drawing_order_layer(
        ntg_object* root,
        ntg_object** buff)
{
    size_t _counter = 1;
    buff[0] = root;

    get_objects_in_drawing_order_layer_internal(root, buff, &_counter);
}

static void get_objects_in_drawing_order_layer_internal(
        ntg_object* root,
        ntg_object** buff,
        size_t* counter)
{
    size_t i;

    ntg_object** children_by_z = buff + (*counter);

    ntg_object_get_children_by_z(root, children_by_z, root->_children.size);
    (*counter) += root->_children.size;

    for(i = 0; i < root->_children.size; i++)
    {
        get_objects_in_drawing_order_layer_internal(children_by_z[i], buff, counter);
    }
}

static void draw_object(ntg_stage* stage, ntg_object* object)
{
    if(!object) return;

    struct ntg_xy abs_pos;
    abs_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(object, ntg_dxy(0, 0)));

    if(object->_dirty & NTG_OBJECT_DIRTY_DRAW)
        return;

    int _status;
    ntg_object_drawing_place_(
            &object->_drawing,
            &stage->_drawing,
            abs_pos,
            &_status);

    _ntg_object_clean(object, NTG_OBJECT_DIRTY_RENDER);
}

static bool draw_layer(ntg_stage* stage, ntg_object* root, sarena* arena)
{
    if(!root) return false;

    size_t tree_size = ntg_object_get_tree_size(root);
    if(tree_size == 0) return false;

    ntg_object** buff = sarena_calloc(arena, sizeof(ntg_object*) * tree_size);
    if(!buff) return true;

    get_objects_in_drawing_order_layer(root, buff);

    size_t i;
    for(i = 0; i < tree_size; i++)
        draw_object(stage, buff[i]);

    return false;
}
