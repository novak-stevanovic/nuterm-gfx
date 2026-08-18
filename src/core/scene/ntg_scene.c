#include <stdlib.h>
#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include "core/scene/ntg_focus_manager.h"
#include <string.h>

#define DEBUG 0

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_scene_layout_data
{
    ntg_scene* scene;
    sarena* arena;
    bool new_it, stay_dirty;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static bool
layout_layer(ntg_scene* scene, ntg_object* root, unsigned int it, sarena* arena);

static void collect_layers_by_z_internal(
        ntg_scene* scene,
        ntg_object* it_root,
        ntg_object** out_layers,
        size_t* counter,
        size_t cap);

static inline bool new_it(uint32_t dirty_flags, unsigned int curr_phase);
static void hmeasure_fn(ntg_object* object, void* _layout_data);
static void hconstrain_fn(ntg_object* object, void* _layout_data);
static void vmeasure_fn(ntg_object* object, void* _layout_data);
static void vconstrain_fn(ntg_object* object, void* _layout_data);
static void arrange_fn(ntg_object* object, void* _layout_data);
static void draw_fn(ntg_object* object, void* _layout_data);
static void finalize_fn(ntg_object* object, void* _layout_data);

NTG_OBJECT_DEF_TRAVERSE_POSTORDER(hmeasure_tree, hmeasure_fn)
NTG_OBJECT_DEF_TRAVERSE_PREORDER(hconstrain_tree, hconstrain_fn)
NTG_OBJECT_DEF_TRAVERSE_POSTORDER(vmeasure_tree, vmeasure_fn)
NTG_OBJECT_DEF_TRAVERSE_PREORDER(vconstrain_tree, vconstrain_fn)
NTG_OBJECT_DEF_TRAVERSE_POSTORDER(arrange_tree, arrange_fn)
NTG_OBJECT_DEF_TRAVERSE_POSTORDER(draw_tree, draw_fn)
NTG_OBJECT_DEF_TRAVERSE_POSTORDER(finalize_tree, finalize_fn)

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static const struct ntg_scene_vtable VTABLE_EMPTY = {0};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static void init_default(ntg_scene* scene)
{
    (*scene) = (ntg_scene) {0};
}

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_scene_init(
        ntg_scene* scene,
        const struct ntg_focus_scope_keybinds* init_scope_keybinds,
        unsigned int max_it)
{
    return ntg_scene_init_override(
            scene,
            &NTG_SCENE_VTABLE_DEFAULT,
            init_scope_keybinds,
            max_it);
}

void ntg_scene_deinit(ntg_scene* scene)
{
    if(!scene) return;

    if(scene->_stage)
        (void)ntg_stage_set_scene(scene->_stage, NULL);

    _ntg_focus_manager_deinit(scene->_fm);
    free(scene->_fm);

    init_default(scene);
}

void ntg_scene_deinit_void(void* _scene)
{
    if(!_scene) return;

    ntg_scene_deinit(_scene);
}

void ntg_scene_mark_dirty(ntg_scene* scene)
{
    if(!scene) return;

    scene->_dirty = true;

    if(scene->_stage)
    {
        ntg_stage_mark_dirty(scene->_stage);
    }
}

int ntg_scene_hit_test(
        ntg_scene* scene,
        struct ntg_xy pos,
        struct ntg_xy* out_object_pos,
        enum ntg_object_hit_result* out_hit,
        ntg_object** out_object)
{
    if(!scene)
        return NTG_ERR_INV_ARG;

    ntg_set_out(out_object, NULL);

    if(out_object_pos)
        (*out_object_pos) = ntg_xy(0, 0);

    size_t layer_count = ntg_scene_collect_layers_by_z(scene, NULL, 0);
    if(layer_count == 0)
        return 0;

    ntg_object** layers = malloc(layer_count * sizeof(ntg_object*));
    if(!layers)
        return NTG_ERR_ALLOC_FAIL;

    ntg_scene_collect_layers_by_z(scene, layers, layer_count);

    size_t i = layer_count;
    struct ntg_dxy it_adj_pos_dxy;
    struct ntg_xy it_adj_pos;
    // struct ntg_xy _out_object_pos;
    // enum ntg_object_hit_result _hit;
    ntg_object* hit = NULL;
    while(i > 0)
    {
        i--;
        it_adj_pos_dxy = ntg_object_map_from_scene(layers[i], ntg_dxy_from_xy(pos));

        if((it_adj_pos_dxy.x < 0) || (it_adj_pos_dxy.y < 0))
            continue;

        it_adj_pos = ntg_xy_from_dxy(it_adj_pos_dxy);

        hit = ntg_object_hit_test(layers[i], it_adj_pos, out_object_pos, out_hit);
        if(hit) break;
    }

    free(layers);

    ntg_set_out(out_object, hit);
    return 0;
}

size_t ntg_scene_collect_layers_by_z(
        ntg_scene* scene,
        ntg_object** out_layers,
        size_t cap)
{
    if(!scene) return 0;

    if(!scene->_root)
        return 0;

    if(!out_layers) cap = SIZE_MAX;
    size_t counter = 0;
    collect_layers_by_z_internal(scene,
            scene->_root,
            out_layers, &counter, cap);

    return counter;
}

int ntg_scene_set_root(ntg_scene* scene, ntg_object* root)
{
    if(!scene)
        return NTG_ERR_INV_ARG;

    ntg_object* old_root = scene->_root;

    if(old_root) 
    {
        _ntg_object_root_set_scene(old_root, NULL);
        
        _ntg_scene_rm_object_tree(scene, old_root);
    }

    if(root)
    {
        
        if(ntg_object_is_true_root(root)) 
        {
            ntg_scene* scene = ntg_object_get_scene_(root);
            if(scene)
            {
                (void)ntg_scene_set_root(scene, NULL);
            }
        }
        else if(ntg_object_is_root(root)) 
        {
            ntg_object_unanchor(root);
        }

        _ntg_scene_add_object_tree(scene, root);
        _ntg_object_root_set_scene(root, scene);
    }

    scene->_root = root;
        
    if(old_root)
        _ntg_scene_unregister_tree(scene, old_root);

    if(root)
        _ntg_scene_register_tree(scene, root);

    if(scene->hooks.on_root_chng_fn)
        scene->hooks.on_root_chng_fn(scene, old_root, root);

    return 0;
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_scene_feed_key(ntg_scene* scene, struct nt_key_event key)
{
    if(!scene) return false;

    bool handled = false;

    if(scene->__vtable && scene->__vtable->handle_key_fn)
        handled = scene->__vtable->handle_key_fn(scene, key);

    if(scene->hooks.on_key_fn)
        scene->hooks.on_key_fn(scene, key);

    return handled;
}

bool ntg_scene_feed_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    if(!scene) return false;

    bool handled = false;

    if(scene->__vtable && scene->__vtable->handle_mouse_fn)
        handled = scene->__vtable->handle_mouse_fn(scene, mouse);

    if(scene->hooks.on_mouse_fn)
        scene->hooks.on_mouse_fn(scene, mouse);

    return handled;
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

int ntg_scene_init_override(
        ntg_scene* scene,
        const struct ntg_scene_vtable* vtable,
        const struct ntg_focus_scope_keybinds* init_scope_keybinds,
        unsigned int max_it)
{
    if(!scene || !max_it)
        return NTG_ERR_INV_ARG;

    init_default(scene);

    scene->__vtable = (vtable ? vtable : &VTABLE_EMPTY);

    scene->_fm = malloc(sizeof(ntg_focus_manager));
    if(!scene->_fm)
        return NTG_ERR_ALLOC_FAIL;

    int _status = _ntg_focus_manager_init(scene->_fm, scene, init_scope_keybinds);

    if(_status != 0)
    {
        free(scene->_fm); 
        scene->_fm = NULL;

        return _status;
    }

    scene->__max_it = max_it;
    return 0;
}

bool ntg_scene_dispatch_key_fn(ntg_scene* scene, struct nt_key_event key)
{
    if(!scene) return false;

    return ntg_focus_manager_feed_key(scene->_fm, key);
}

bool ntg_scene_dispatch_mouse_fn(ntg_scene* scene, struct nt_mouse_event mouse)
{
    if(!scene) return false;

    return ntg_focus_manager_feed_mouse(scene->_fm, mouse);
}

const struct ntg_scene_vtable NTG_SCENE_VTABLE_DEFAULT = {
    .handle_key_fn = ntg_scene_dispatch_key_fn,
    .handle_mouse_fn = ntg_scene_dispatch_mouse_fn
};

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

int _ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size)
{
    if(!scene)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    struct ntg_xy old_size = scene->_size;

    if(ntg_xy_are_eql(old_size, size))
        return 0;

    scene->_size = size;
    ntg_scene_mark_dirty(scene);
    
    if(scene->hooks.on_size_chng_fn)
        scene->hooks.on_size_chng_fn(scene, old_size, size);

    return 0;
}

bool _ntg_scene_layout(ntg_scene* scene, sarena* arena)
{
    if(!scene) return false;

    size_t layer_count = ntg_scene_collect_layers_by_z(scene, NULL, 0);

    if(layer_count == 0) return false;

    ntg_object** layers = sarena_calloc(arena, sizeof(ntg_object*) * layer_count);
    if(!layers) return true;
    ntg_scene_collect_layers_by_z(scene, layers, layer_count);

    size_t i;
    bool relayout = false;
    for(i = 0; i < layer_count; i++)
    {
        bool layer_relayout = layout_layer(scene, layers[i], 0, arena);
        relayout = relayout || layer_relayout;
    }

    return relayout;
}

void _ntg_scene_clean(ntg_scene* scene)
{
    if(!scene) return;

    scene->_dirty = false;
}

void _ntg_scene_set_stage(ntg_scene* scene, ntg_stage* stage)
{
    if(!scene) return;

    if(scene->_stage == stage) return;

    scene->_stage = stage;
    if(stage)
        ntg_scene_mark_dirty(scene);
}

void _ntg_scene_on_stage_enter(ntg_scene* scene, ntg_stage* stage)
{
    if(!scene) return;

    if(scene->hooks.on_stage_enter_fn)
        scene->hooks.on_stage_enter_fn(scene, stage);
}

void _ntg_scene_on_stage_leave(ntg_scene* scene, ntg_stage* stage)
{
    if(!scene) return;

    if(scene->hooks.on_stage_leave_fn)
        scene->hooks.on_stage_leave_fn(scene, stage);
}

void _ntg_scene_add(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);
    ntg_scene_mark_dirty(scene); 
}

void _ntg_scene_rm(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    ntg_scene_mark_dirty(scene);

    _ntg_focus_manager_on_scene_object_rm(scene->_fm, object);
}

void _ntg_scene_register(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    ntg_scene_mark_dirty(scene); 

    _ntg_object_scene_enter(object, scene);

    if(scene->hooks.on_object_register_fn)
        scene->hooks.on_object_register_fn(scene, object);

    if(ntg_object_is_only_layer_root(object))
    {
        ntg_scene* scene = ntg_object_get_scene_(object);

        if(scene)
        {
            if(scene->hooks.on_layer_add_fn)
                scene->hooks.on_layer_add_fn(scene, object);
        }
    }

    _ntg_object_on_scene_enter(object, scene);
}

void _ntg_scene_unregister(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    _ntg_object_scene_leave(object, scene);

    if(scene->hooks.on_object_unregister_fn)
        scene->hooks.on_object_unregister_fn(scene, object);

    if(ntg_object_is_only_layer_root(object))
    {
        if(scene)
        {
            if(scene->hooks.on_layer_rm_fn)
                scene->hooks.on_layer_rm_fn(scene, object);
        }
    }

    _ntg_object_on_scene_leave(object, scene);
}

void _ntg_scene_add_object_tree(ntg_scene* scene, ntg_object* root)
{
    if(!scene || !root) return;

    _ntg_scene_add(scene, root);

    size_t i;
    for(i = 0; i < root->_children.size; i++)
    {
        ntg_object* child = root->_children.data[i];
        _ntg_scene_add_object_tree(scene, child);
    }

    for(i = 0; i < root->_anchored.size; i++)
    {
        ntg_object* layer = root->_anchored.data[i];
        _ntg_scene_add_object_tree(scene, layer);
    }
}

void _ntg_scene_rm_object_tree(ntg_scene* scene, ntg_object* root)
{
    if(!scene || !root) return;

    _ntg_scene_rm(scene, root);

    size_t i;
    for(i = 0; i < root->_children.size; i++)
    {
        ntg_object* child = root->_children.data[i];
        _ntg_scene_rm_object_tree(scene, child);
    }

    for(i = 0; i < root->_anchored.size; i++)
    {
        ntg_object* layer = root->_anchored.data[i];
        _ntg_scene_rm_object_tree(scene, layer);
    }
}

void _ntg_scene_register_tree(ntg_scene* scene, ntg_object* root)
{
    if(!scene || !root) return;

    _ntg_scene_register(scene, root);

    size_t i;
    for(i = 0; i < root->_children.size; i++)
    {
        ntg_object* child = root->_children.data[i];
        _ntg_scene_register_tree(scene, child);
    }

    for(i = 0; i < root->_anchored.size; i++)
    {
        ntg_object* layer = root->_anchored.data[i];
        _ntg_scene_register_tree(scene, layer);
    }
}

void _ntg_scene_unregister_tree(ntg_scene* scene, ntg_object* root)
{
    if(!scene || !root) return;

    _ntg_scene_unregister(scene, root);

    size_t i;
    for(i = 0; i < root->_children.size; i++)
    {
        ntg_object* child = root->_children.data[i];
        _ntg_scene_unregister_tree(scene, child);
    }

    for(i = 0; i < root->_anchored.size; i++)
    {
        ntg_object* layer = root->_anchored.data[i];
        _ntg_scene_unregister_tree(scene, layer);
    }
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static void collect_layers_by_z_internal(
        ntg_scene* scene,
        ntg_object* it_root,
        ntg_object** out_layers,
        size_t* counter,
        size_t cap)
{
    const ntg_objptr_vec* children = &(it_root->_children);
    const ntg_objptr_vec* anchored = &(it_root->_anchored);

    size_t i;
    ntg_object* it_obj;

    bool true_root = (it_root->_parent == NULL);
    
    if(true_root)
    {
        if(out_layers && (cap > 0))
        {
            size_t stored_count = _min2_size(*counter, cap);

            for(i = 0; i < stored_count; i++)
            {
                it_obj = out_layers[i];
                
                if(it_obj->_layout_opts.z_index > it_root->_layout_opts.z_index)
                    break;
            }

            if(i < cap)
            {
                size_t move_count = (stored_count < cap) ?
                    (stored_count - i) : (cap - i - 1);

                if(move_count > 0)
                {
                    memmove(out_layers + i + 1,
                            out_layers + i,
                            move_count * sizeof(ntg_object*));
                }
                out_layers[i] = it_root;
            }
        }

        (*counter)++;
    }
    
    for(i = 0; i < anchored->size; i++)
    {
        collect_layers_by_z_internal(scene, anchored->data[i],
            out_layers, counter, cap);
    }

    for(i = 0; i < children->size; i++)
    {
        collect_layers_by_z_internal(scene, children->data[i],
                out_layers, counter, cap);
    }
}

static bool 
layout_layer(ntg_scene* scene, ntg_object* root, unsigned int it, sarena* arena)
{
    if(!root) return false;

    const struct ntg_anchor_policy* policy = root->_anchor_policy;
    ntg_object* base = root->_base;

    struct ntg_scene_layout_data layout_data = {
        .scene = scene,
        .arena = arena,
        .new_it = false,
        .stay_dirty = false
    };

    hmeasure_tree(root, &layout_data);
    
    struct ntg_anchor_constrain_ctx constrain_ctx = {
        .root = root,
        .base = base
    };

    size_t hsize = _ntg_anchor_policy_constrain(
            policy,
            NTG_ORIENT_H,
            &constrain_ctx,
            arena);
    hsize = _clamp_size(0, hsize, scene->_size.x);

    if(root->_size.x != hsize)
    {
        _ntg_object_root_set_hsize(root, hsize);
    }

    hconstrain_tree(root, &layout_data);

    vmeasure_tree(root, &layout_data);

    size_t vsize = _ntg_anchor_policy_constrain(
            policy,
            NTG_ORIENT_V,
            &constrain_ctx,
            arena);
    vsize = _clamp_size(0, vsize, scene->_size.y);

    if(root->_size.y != vsize)
    {
        _ntg_object_root_set_vsize(root, vsize);
    }

    vconstrain_tree(root, &layout_data);

    struct ntg_xy size = ntg_xy(hsize, vsize);
    
    struct ntg_anchor_arrange_ctx arrange_ctx = {
        .base = base,
        .root = root,
        .size = size,
    };

    struct ntg_xy pos = _ntg_anchor_policy_arrange(
            policy,
            &arrange_ctx,
            arena);
    pos = ntg_xy_pos_clamp(pos, size, scene->_size);

    pos.x -= _sub2_size(pos.x + size.x, scene->_size.x);
    pos.y -= _sub2_size(pos.y + size.y, scene->_size.y);

    if(!ntg_xy_are_eql(root->_pos, pos))
    {
        _ntg_object_root_set_pos(root, pos);
    }

    arrange_tree(root, &layout_data);
    draw_tree(root, &layout_data);

    bool stay_dirty = false;

    if(layout_data.new_it)
    {
        if((it + 1) < scene->__max_it)
            stay_dirty = layout_layer(scene, root, it + 1, arena);
        else
            stay_dirty = true;
    }
    else
        stay_dirty = layout_data.stay_dirty;
    
    /* Finalize layout */
    
    if(it == 0)
        finalize_tree(root, &layout_data);

    return stay_dirty;
}

static uint32_t masks[] = {
        0x01, // HM
        0x03, // HC
        0x07, // VM
        0x0F, // VC
        0x1F, // A
        0x3F // D
};

static inline bool new_it(uint32_t dirty_flags, unsigned int curr_phase)
{
    if(curr_phase > (sizeof(masks) / sizeof(uint32_t)))
        return false;

    return ((dirty_flags & masks[curr_phase]) != 0);
}

static void hmeasure_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_HMEASURE)
    {
        ntg_log_log("NTG_SCENE | HM | %p", object);

        uint32_t _relayout = 0;
        int _status = _ntg_object_hmeasure(object, arena, &_relayout);
        if(_status)
            layout_data->stay_dirty = true;
        else
            _ntg_object_clean(object, NTG_OBJECT_DIRTY_HMEASURE);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 0);
    }
    else
    {
        ntg_log_log("NTG_SCENE | HM SKIPPED | %p", object);
    }
}

static void hconstrain_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_HCONSTRAIN)
    {
        ntg_log_log("NTG_SCENE | HC | %p", object);

        uint32_t _relayout = 0;
        int _status = _ntg_object_hconstrain(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            _ntg_object_clean(object, NTG_OBJECT_DIRTY_HCONSTRAIN);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 1);
    }
    else
    {
        ntg_log_log("NTG_SCENE | HC SKIPPED | %p", object);
    }
}

static void vmeasure_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_VMEASURE)
    {
        ntg_log_log("NTG_SCENE | VM | %p", object);

        uint32_t _relayout = 0;
        int _status = _ntg_object_vmeasure(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            _ntg_object_clean(object, NTG_OBJECT_DIRTY_VMEASURE);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 2);
    }
    else
    {
        ntg_log_log("NTG_SCENE | VM SKIPPED | %p", object);
    }
}

static void vconstrain_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_VCONSTRAIN)
    {
        ntg_log_log("NTG_SCENE | VC | %p", object);

        uint32_t _relayout = 0;
        int _status = _ntg_object_vconstrain(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            _ntg_object_clean(object, NTG_OBJECT_DIRTY_VCONSTRAIN);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 3);
    }
    else
    {
        ntg_log_log("NTG_SCENE | VC SKIPPED | %p", object);
    }
}

static void arrange_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_ARRANGE)
    {
        ntg_log_log("NTG_SCENE | A | %p", object);

        uint32_t _relayout = 0;
        int _status = _ntg_object_arrange(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            _ntg_object_clean(object, NTG_OBJECT_DIRTY_ARRANGE);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 4);
    }
    else
    {
        ntg_log_log("NTG_SCENE | A SKIPPED | %p", object);
    }
}

static void draw_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_DRAW)
    {
        ntg_log_log("NTG_SCENE | D | %p", object);

        uint32_t _relayout = 0;
        int _status = _ntg_object_draw(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            _ntg_object_clean(object, NTG_OBJECT_DIRTY_DRAW);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 5);
    }
    else
    {
        ntg_log_log("NTG_SCENE | D SKIPPED | %p", object);
    }
}

static void finalize_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_LAYOUT_FINALIZE)
    {
        ntg_log_log("NTG_SCENE | F | %p", object);

        _ntg_object_layout_finalize(object, arena);

        _ntg_object_clean(object, NTG_OBJECT_DIRTY_LAYOUT_FINALIZE);
    }
    else
    {
        ntg_log_log("NTG_SCENE | F SKIPPED | %p", object);
    }
}
