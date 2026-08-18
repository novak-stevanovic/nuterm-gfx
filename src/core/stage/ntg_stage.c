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
static void init_default_drawing(ntg_stage* stage);

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

int ntg_stage_init(ntg_stage* stage)
{
    return ntg_stage_init_override(stage, &NTG_STAGE_VTABLE_DEFAULT);
}

void ntg_stage_deinit(ntg_stage* stage)
{
    if(!stage) return;

    if(stage->_in_loop)
    {
        (void)ntg_loop_set_stage(NULL);
    }

    if(stage->_scene)
    {
        (void)ntg_stage_set_scene(stage, NULL);
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

bool ntg_stage_compose(ntg_stage* stage, sarena* arena)
{
    if(!stage || !arena) return false;

    struct ntg_xy size = stage->_size;

    int _status;
    if(!ntg_xy_are_eql(ntg_stage_drawing_get_size(&stage->_drawing), size))
    {
        struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
        _status = ntg_stage_drawing_set_size(&stage->_drawing, size, size_cap);
        if(_status)
            return true;
    }

    if(stage->_scene && stage->_scene->_dirty)
    {
        if(!_ntg_scene_layout(stage->_scene, arena))
            _ntg_scene_clean(stage->_scene);
    }

    if(ntg_xy_size_is_zero(size)) return false;

    if(!stage->_scene) return false;
    if(!stage->_scene->_root) return false;

    size_t layer_count = ntg_scene_collect_layers_by_z(stage->_scene, NULL, 0);

    ntg_object** layers = sarena_calloc(arena, sizeof(ntg_object*) * layer_count);
    if(!layers)
        return true;

    ntg_scene_collect_layers_by_z(stage->_scene, layers, layer_count);

    /* Drawing is happening so default init cells */
    init_default_drawing(stage);

    bool rval = false;
    size_t i;
    for(i = 0; i < layer_count; i++)
    {
        bool layer_redraw = draw_layer(stage, layers[i], arena);
        rval = rval || layer_redraw;
    }

    return rval;
}


int ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    if(!stage)
        return NTG_ERR_INV_ARG;

    if(stage->_scene == scene) return 0;

    ntg_scene* old_scene = stage->_scene;
    ntg_stage* old_scene_stage = (old_scene ? old_scene->_stage : NULL);
    ntg_stage* old_stage = (scene ? scene->_stage : NULL);

    if(old_scene)
    {
        _ntg_scene_set_stage(old_scene, NULL);

        /* Can only fail if size exceeds NTG_SIZE_MAX */
        (void)_ntg_scene_set_size(old_scene, ntg_xy(0, 0));
    }

    if(scene)
    {
        if(old_stage)
        {
            (void)ntg_stage_set_scene(old_stage, NULL);
        }

        _ntg_scene_set_stage(scene, stage);

        /* Can only fail if size exceeds NTG_SIZE_MAX */
        int status = _ntg_scene_set_size(scene, stage->_size);
        if(status != 0)
            return status;
        ntg_scene_mark_dirty(scene);
    }

    stage->_scene = scene;

    if(stage->hooks.on_scene_chng_fn)
        stage->hooks.on_scene_chng_fn(stage, old_scene, scene);

    if(old_scene)
        _ntg_scene_on_stage_leave(old_scene, old_scene_stage);

    if(scene)
        _ntg_scene_on_stage_enter(scene, stage);

    return 0;
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

int ntg_stage_init_override(
        ntg_stage* stage,
        const struct ntg_stage_vtable* vtable)
{
    if(!stage)
        return NTG_ERR_INV_ARG;

    init_default(stage);
    stage->__vtable = (vtable ? vtable : &VTABLE_EMPTY);

    int _status = ntg_stage_drawing_init(&stage->_drawing);

    if(_status != 0)
    {
        init_default(stage);
        return _status;
    }

    return 0;
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

int _ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size)
{
    if(!stage)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    if(ntg_xy_are_eql(stage->_size, size))
        return 0;

    struct ntg_xy old_size = stage->_size;

    stage->_size = size;
    ntg_stage_mark_dirty(stage);

    int _status;

    if(stage->_scene)
    {
        _status = _ntg_scene_set_size(stage->_scene, size);
        if(_status != 0)
            return _status;
    }

    if(stage->hooks.on_size_chng_fn)
        stage->hooks.on_size_chng_fn(stage, old_size, size);

    return 0;
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
    if(!stage) return;

    (*stage) = (ntg_stage) {0};
}

static void init_default_drawing(ntg_stage* stage)
{
    if(!stage) return;

    struct ntg_xy size = ntg_stage_drawing_get_size(&stage->_drawing);

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

    ntg_object_get_children_by_z(root, children_by_z, ntg_objptr_vec_size(&root->_children));
    (*counter) += ntg_objptr_vec_size(&root->_children);

    for(i = 0; i < ntg_objptr_vec_size(&root->_children); i++)
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

    (void)ntg_object_drawing_place_(
            &object->_drawing,
            &stage->_drawing,
            abs_pos);

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
