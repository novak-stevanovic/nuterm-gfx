#include <stdlib.h>
#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include "core/scene/ntg_focus_manager.h"

#define DEBUG 0

#define LAYER_LAYOUT_MAX_IT 10

/* ========================================================================== */
/* TYPE DEFINITIONS */
/* ========================================================================== */

struct ntg_scene_layout_data
{
    ntg_scene* scene;
    sarena* arena;
    bool repeat;
};

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* LAYOUT */
/* -------------------------------------------------------------------------- */

static void 
layout_layer(ntg_scene* scene, ntg_object* root, unsigned int it, sarena* arena);

static void collect_layers_by_z_internal(
        ntg_scene* scene,
        ntg_object* it_root,
        ntg_object** out_layers,
        size_t* counter,
        size_t cap);

static void hmeasure_fn(ntg_object* object, void* _layout_data);
static void hconstrain_fn(ntg_object* object, void* _layout_data);
static void fixup_fn(ntg_object* object, void* _layout_data);
static void vmeasure_fn(ntg_object* object, void* _layout_data);
static void vconstrain_fn(ntg_object* object, void* _layout_data);
static void arrange_fn(ntg_object* object, void* _layout_data);
static void draw_fn(ntg_object* object, void* _layout_data);

NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(hmeasure_tree, hmeasure_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(hconstrain_tree, hconstrain_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(vmeasure_tree, vmeasure_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(vconstrain_tree, vconstrain_fn);
NTG_OBJECT_TRAVERSE_PREORDER_DEFINE(fixup_tree, fixup_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(arrange_tree, arrange_fn);
NTG_OBJECT_TRAVERSE_POSTORDER_DEFINE(draw_tree, draw_fn);

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

static void init_default(ntg_scene* scene)
{
    (*scene) = (ntg_scene) {0};
    scene->hooks.on_key_fn = ntg_scene_dispatch_key;
    scene->hooks.on_mouse_fn = ntg_scene_dispatch_mouse;
}

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

void ntg_scene_init(ntg_scene* scene, int* out_status)
{
    ntg_init_status(out_status); 

    if(!scene)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    init_default(scene);

    scene->_fm = malloc(sizeof(ntg_focus_manager));
    if(!scene->_fm)
        ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

    int _status;
    ntg_focus_manager_init(scene->_fm, scene, &_status);

    if(_status != NTG_SUCCESS)
    {
        free(scene->_fm); 
        scene->_fm = NULL;

        switch(_status)
        {
            case NTG_ERR_ALLOC_FAIL:
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            default:
                ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
        }
    }

}

void ntg_scene_deinit(ntg_scene* scene)
{
    if(!scene) return;

    if(scene->_stage)
        ntg_stage_set_scene(scene->_stage, NULL, NULL);

    ntg_focus_manager_deinit(scene->_fm);
    free(scene->_fm);

    init_default(scene);
}

void ntg_scene_deinit_(void* _scene)
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

ntg_object* ntg_scene_hit_test(
        ntg_scene* scene,
        struct ntg_xy pos,
        struct ntg_xy* out_object_pos,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!scene)
        ntg_return(NULL, out_status, NTG_ERR_INVALID_ARG);

    if(out_object_pos)
        (*out_object_pos) = ntg_xy(0, 0);

    size_t layer_count = ntg_scene_collect_layers_by_z(scene, NULL, 0);
    if(layer_count == 0)
        return NULL;

    ntg_object** layers = malloc(layer_count * sizeof(ntg_object*));
    if(!layers)
        ntg_return(NULL, out_status, NTG_ERR_ALLOC_FAIL);

    ntg_scene_collect_layers_by_z(scene, layers, layer_count);

    int i;
    struct ntg_xy it_adj_pos;
    struct ntg_xy _out_object_pos;
    ntg_object* hit = NULL;
    for(i = layer_count - 1; i >= 0; i--)
    {
        it_adj_pos = ntg_xy_from_dxy(
                ntg_object_map_from_scene(layers[i],
                ntg_dxy_from_xy(pos)));

        hit = ntg_object_hit_test(layers[i], it_adj_pos, &_out_object_pos);
        if(hit) break;
    }

    free(layers);

    if(hit)
    {
        if(out_object_pos)
            (*out_object_pos) = _out_object_pos;

        return hit;
    }
    else
        return NULL;
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

void ntg_scene_set_root(ntg_scene* scene, ntg_object* root, int* out_status)
{
    ntg_init_status(out_status);

    if(!scene)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_object* old_root = scene->_root;

    // Remove old root
    if(old_root) // already has root?
    {
        _ntg_object_root_set_scene(old_root, NULL);
        // If an element is focused, _ntg_scene_rm_object_tree() will take care of it
        _ntg_scene_rm_object_tree(scene, old_root);
    }

    // Update new root state
    if(root)
    {
        // If root has old scene, remove
        if(ntg_object_is_true_root(root)) // true root
        {
            ntg_scene* scene = ntg_object_get_scene_(root);
            if(scene)
            {
                ntg_scene_set_root(scene, NULL, NULL);
            }
        }
        else if(ntg_object_is_root(root)) // layer
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
}

/* -------------------------------------------------------------------------- */
/* EVENT */
/* -------------------------------------------------------------------------- */

bool ntg_scene_dispatch_key(ntg_scene* scene, struct nt_key_event key)
{
    if(!scene) return false;

    return ntg_focus_manager_feed_key(scene->_fm, key);
}

bool ntg_scene_dispatch_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    if(!scene) return false;

    return ntg_focus_manager_feed_mouse(scene->_fm, mouse);
}

bool ntg_scene_feed_key(ntg_scene* scene, struct nt_key_event key)
{
    if(!scene) return false;

    if(scene->hooks.on_key_fn)
        return scene->hooks.on_key_fn(scene, key);
    else
        return false;
}

bool ntg_scene_feed_mouse(ntg_scene* scene, struct nt_mouse_event mouse)
{
    if(!scene) return false;

    if(scene->hooks.on_mouse_fn)
        return scene->hooks.on_mouse_fn(scene, mouse);
    else
        return false;
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* LAYOUT */
/* -------------------------------------------------------------------------- */

void _ntg_scene_set_size(ntg_scene* scene, struct ntg_xy size)
{
    if(!scene) return;

    struct ntg_xy old_size = scene->_size;

    if(ntg_xy_are_equal(old_size, size))
        return;

    scene->_size = size;
    ntg_scene_mark_dirty(scene);
    
    if(scene->hooks.on_size_chng_fn)
        scene->hooks.on_size_chng_fn(scene, old_size, size);
}

void _ntg_scene_layout(ntg_scene* scene, sarena* arena)
{
    if(!scene) return;

    size_t layer_count = ntg_scene_collect_layers_by_z(scene, NULL, 0);

    ntg_object** layers = sarena_calloc(arena, sizeof(ntg_object*) * layer_count);
    ntg_scene_collect_layers_by_z(scene, layers, layer_count);

    size_t i;
    for(i = 0; i < layer_count; i++)
        layout_layer(scene, layers[i], 0, arena);
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
}

void _ntg_scene_add(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);
    ntg_scene_mark_dirty(scene); // just in case
}

void _ntg_scene_rm(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    ntg_scene_mark_dirty(scene);

    ntg_focus_manager_invalidate(scene->_fm, object);
}

void _ntg_scene_register(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);
    ntg_scene_mark_dirty(scene); // just in case

    if(scene->hooks.on_object_register_fn)
        scene->hooks.on_object_register_fn(scene, object);

    if(object->hooks.on_scene_chng_fn)
        object->hooks.on_scene_chng_fn(object, NULL, scene);

    if(ntg_object_is_only_layer_root(object))
    {
        ntg_scene* scene = ntg_object_get_scene_(object);

        if(scene)
        {
            if(scene->hooks.on_layer_add_fn)
                scene->hooks.on_layer_add_fn(scene, object);
        }
    }
}

void _ntg_scene_unregister(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    if(scene->hooks.on_object_unregister_fn)
        scene->hooks.on_object_unregister_fn(scene, object);

    if(object->hooks.on_scene_chng_fn)
        object->hooks.on_scene_chng_fn(object, scene, NULL);

    if(ntg_object_is_only_layer_root(object))
    {
        if(scene)
        {
            if(scene->hooks.on_layer_rm_fn)
                scene->hooks.on_layer_rm_fn(scene, object);
        }
    }
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
/* LAYOUT */
/* -------------------------------------------------------------------------- */

static void collect_layers_by_z_internal(
        ntg_scene* scene,
        ntg_object* it_root,
        ntg_object** out_layers,
        size_t* counter,
        size_t cap)
{
    // assert(scene);
    // assert(counter);
    // assert(it_root);

    const ntg_object_vec* children = &(it_root->_children);
    const ntg_object_vec* anchored = &(it_root->_anchored);

    size_t i;
    ntg_object* it_obj;

    bool true_root = (it_root->_parent == NULL);
    
    if(true_root)
    {
        if(out_layers)
        {
            for(i = 0; i < (*counter); i++)
            {
                it_obj = out_layers[i];
                
                if(it_obj->_layout_opts.z_index > it_root->_layout_opts.z_index)
                    break;
            }

            if(cap > (*counter))
            {
                // assert(i <= (*counter));
                if(i < (*counter))
                {
                    memmove(out_layers + i + 1,
                            out_layers + i,
                            ((*counter) - i) * sizeof(ntg_object*));
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

static void 
layout_layer(ntg_scene* scene, ntg_object* root, unsigned int it, sarena* arena)
{
    const struct ntg_anchor_policy* policy = root->_anchor_policy;
    ntg_object* base = root->_base;

    // Sentinel node just returns
    if(!root) return;

    struct ntg_scene_layout_data layout_data = {
        .scene = scene,
        .arena = arena,
        .repeat = false
    };

    // H MEASURE

    if(it == 0)
        hmeasure_tree(root, &layout_data);

    // H CONSTRAIN
    
    size_t hsize = 0;
    if(policy->constrain_fn)
    {
        struct ntg_anchor_constrain_ctx hconstrain_ctx = {
            .root = root,
            .base = base
        };
        hsize = policy->constrain_fn(policy->data,
                NTG_ORIENT_H, &hconstrain_ctx, arena);

        hsize = _clamp_size(0, hsize, scene->_size.x);
    }

    if(root->_size.x != hsize)
    {
        _ntg_object_root_set_hsize(root, hsize);
    }

    hconstrain_tree(root, &layout_data);

    // V MEASURE

    vmeasure_tree(root, &layout_data);

    // V CONSTRAIN
    
    size_t vsize = 0;
    if(policy->constrain_fn)
    {
        struct ntg_anchor_constrain_ctx vconstrain_ctx = {
            .root = root,
            .base = base
        };
        vsize = policy->constrain_fn(policy->data,
                NTG_ORIENT_V, &vconstrain_ctx, arena);

        vsize = _clamp_size(0, vsize, scene->_size.y);
    }

    if(root->_size.y != vsize)
    {
        _ntg_object_root_set_vsize(root, vsize);
    }

    vconstrain_tree(root, &layout_data);

    struct ntg_xy size = ntg_xy(hsize, vsize);
    
    // FIX UP

    fixup_tree(root, &layout_data);

    if(layout_data.repeat)
    {
        layout_layer(scene, root, it + 1, arena);
    }
    else
    {
        struct ntg_xy pos = ntg_xy(0, 0);
        if(policy->arrange_fn)
        {
            struct ntg_anchor_arrange_ctx arrange_ctx = {
                .base = base,
                .root = root,
                .size = size,
            };

            pos = policy->arrange_fn(policy->data, &arrange_ctx, arena);
            pos = ntg_xy_pos_clamp(pos, size, scene->_size);

            pos.x -= _ssub_size(pos.x + size.x, scene->_size.x);
            pos.y -= _ssub_size(pos.y + size.y, scene->_size.y);
        }

        if(!ntg_xy_are_equal(root->_pos, pos))
        {
            _ntg_object_root_set_pos(root, pos);
        }

        arrange_tree(root, &layout_data);
        draw_tree(root, &layout_data);
    }
}

static void hmeasure_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_HMEASURE)
    {
        ntg_log_log("NTG_DEF_SCENE | M1 | %p", object);
        _ntg_object_hmeasure(object, arena);

        _ntg_object_clean(object, NTG_OBJECT_DIRTY_HMEASURE);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(HM)", object);
    }
}

static void hconstrain_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_HCONSTRAIN)
    {
        ntg_log_log("NTG_DEF_SCENE | C1 | %p", object);
        _ntg_object_hconstrain(object, arena);
        _ntg_object_clean(object, NTG_OBJECT_DIRTY_HCONSTRAIN);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(HC)", object);
    }
}

static void vmeasure_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_VMEASURE)
    {
        ntg_log_log("NTG_DEF_SCENE | M2 | %p", object);
        _ntg_object_vmeasure(object, arena);

        _ntg_object_clean(object, NTG_OBJECT_DIRTY_VMEASURE);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(VM)", object);
    }
}

static void vconstrain_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_VCONSTRAIN)
    {
        ntg_log_log("NTG_DEF_SCENE | C2 | %p", object);
        _ntg_object_vconstrain(object, arena);
        _ntg_object_clean(object, NTG_OBJECT_DIRTY_VCONSTRAIN);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(VC)", object);
    }
}

static void fixup_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(_ntg_object_fixup(object, arena))
    {
        ntg_log_log("Object: %p demanded repeat", object);
        layout_data->repeat = true;
    }
}

static void arrange_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_ARRANGE)
    {
        ntg_log_log("NTG_DEF_SCENE | A | %p", object);
        _ntg_object_arrange(object, arena);
        _ntg_object_clean(object, NTG_OBJECT_DIRTY_ARRANGE);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(AR)", object);
    }
}

static void draw_fn(ntg_object* object, void* _layout_data)
{
    struct ntg_scene_layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->_dirty & NTG_OBJECT_DIRTY_DRAW)
    {
        ntg_log_log("NTG_DEF_SCENE | D | %p", object);
        _ntg_object_draw(object, arena);
        _ntg_object_clean(object, NTG_OBJECT_DIRTY_DRAW);
    }
    else
    {
        ntg_log_log("OPTIMIZE: Object %p not dirty(DR)", object);
    }
}
