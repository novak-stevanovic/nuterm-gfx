#include "ntg.h"
#include <assert.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void init_default(ntg_stage* stage);

static void draw_object(ntg_stage* stage, ntg_object* object);
static void draw_object_fn(ntg_object* object, void* _stage);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(draw_object_tree, draw_object_fn);
static void draw_layer(ntg_stage* stage, ntg_object* root, sarena* arena);

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_stage_init(ntg_stage* stage)
{
    assert(stage != NULL);

    init_default(stage);

    ntg_stage_drawing_init(&stage->_drawing);
}

void ntg_stage_deinit(ntg_stage* stage)
{
    if(stage->_loop)
    {
        ntg_loop_set_stage(stage->_loop, NULL);
    }

    if(stage->_scene)
    {
        ntg_stage_set_scene(stage, NULL);
    }

    ntg_stage_drawing_deinit(&stage->_drawing);

    init_default(stage);
}

void ntg_stage_deinit_(void* _stage)
{
    ntg_stage_deinit(_stage);
}

void ntg_stage_mark_dirty(ntg_stage* stage)
{
    stage->_dirty = true;
}

void ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size)
{
    if(!ntg_xy_are_equal(stage->_size, size))
    {
        stage->_size = size;
        ntg_stage_mark_dirty(stage);
    }

    if(stage->_scene)
        ntg_scene_set_size(stage->_scene, size);
}

void ntg_stage_compose(ntg_stage* stage, sarena* arena)
{
    assert(stage != NULL);

    struct ntg_xy size = stage->_size;

    if(!ntg_xy_are_equal(ntg_stage_drawing_get_size(&stage->_drawing), size))
    {
        struct ntg_xy size_cap = ntg_xy(size.x + 20, size.y + 20);
        ntg_stage_drawing_set_size(&stage->_drawing, size, size_cap);
    }

    if(ntg_xy_size_is_zero(size)) return;

    if(stage->_scene && stage->_scene->_dirty)
    {
        ntg_scene_layout(stage->_scene, arena);
        _ntg_scene_clean(stage->_scene);
    }

    if(ntg_xy_size_is_zero(size)) return;

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

    if(!stage->_scene) return;

    size_t layer_count = ntg_scene_get_root_count(stage->_scene);
    if(layer_count == 0) return;

    ntg_object** layers = sarena_malloc(arena,
            sizeof(ntg_object*) * layer_count);
    assert(layers);

    ntg_scene_get_roots_by_z(stage->_scene, layers, layer_count);

    for(i = layer_count; i > 0; i--)
        draw_layer(stage, layers[i - 1], arena);
}

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene)
{
    assert(stage != NULL);
    if(stage->_scene == scene) return;

    ntg_scene* old_scene = stage->_scene;

    if(old_scene)
    {
        _ntg_scene_set_stage(old_scene, NULL);

        ntg_scene_set_size(old_scene, ntg_xy(0, 0));
    }

    stage->_scene = scene;

    if(scene)
    {
        ntg_stage* old_stage = scene->_stage;

        if(old_stage)
        {
            ntg_stage_set_scene(old_stage, NULL);
        }

        _ntg_scene_set_stage(scene, stage);
        ntg_scene_set_size(scene, stage->_size);
        ntg_scene_mark_dirty(scene);
    }
}

bool ntg_stage_dispatch_key(ntg_stage* stage, struct nt_key_event key)
{
    assert(stage != NULL);

    if(stage->_scene)
        return ntg_scene_on_key(stage->_scene, key);
    else
        return false;
}

bool ntg_stage_dispatch_mouse(ntg_stage* stage, struct nt_mouse_event mouse)
{
    assert(stage != NULL);

    if(stage->_scene)
        return ntg_scene_on_mouse(stage->_scene, mouse);
    else
        return false;
}

void ntg_stage_set_on_key_fn(ntg_stage* stage,
        bool (*on_key_fn)(ntg_stage* stage, struct nt_key_event key))
{
    assert(stage != NULL);
    
    stage->__on_key_fn = on_key_fn;
}

bool ntg_stage_on_key(ntg_stage* stage, struct nt_key_event key)
{
    assert(stage != NULL);

    if(stage->__on_key_fn)
        return stage->__on_key_fn(stage, key);
    else
        return false;
}

void ntg_stage_set_on_mouse_fn(ntg_stage* stage,
        bool (*on_mouse_fn)(ntg_stage* stage, struct nt_mouse_event mouse))
{
    assert(stage != NULL);

    stage->__on_mouse_fn = on_mouse_fn;
}

bool ntg_stage_on_mouse(ntg_stage* stage, struct nt_mouse_event mouse)
{
    assert(stage != NULL);

    if(stage->__on_mouse_fn)
        return stage->__on_mouse_fn(stage, mouse);
    else
        return false;
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

void _ntg_stage_set_loop(ntg_stage* stage, ntg_loop* loop)
{
    assert(stage != NULL);

    if(stage->_loop == loop) return;

    stage->_loop = loop;
    if(loop)
        ntg_stage_mark_dirty(stage);
}

void _ntg_stage_clean(ntg_stage* stage)
{
    stage->_dirty = false;
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void init_default(ntg_stage* stage)
{
    (*stage) = (ntg_stage) {0};

    stage->__on_key_fn = ntg_stage_dispatch_key;
    stage->__on_mouse_fn = ntg_stage_dispatch_mouse;
}

static void draw_object(ntg_stage* stage, ntg_object* object)
{
    struct ntg_xy abs_pos; 
    abs_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(object, ntg_dxy(0, 0)));

    struct ntg_xy draw_size = ntg_object_drawing_get_size(&object->_drawing);

    ntg_object_drawing_place_(
            &object->_drawing,
            ntg_xy(0, 0),
            draw_size,
            &stage->_drawing,
            abs_pos);

    _ntg_object_clean(object, NTG_OBJECT_DIRTY_RENDER);
}

static void draw_object_fn(ntg_object* object, void* _stage)
{
    draw_object((ntg_stage*)_stage, object);
}

static void draw_layer(ntg_stage* stage, ntg_object* root, sarena* arena)
{
    if(root) draw_object_tree(root, stage);
}
