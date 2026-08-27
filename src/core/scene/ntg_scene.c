#include <stdlib.h>
#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include "core/scene/ntg_fcs_manager.h"
#include <string.h>
#include <assert.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct layout_data
{
    ntg_scene* scene;
    sarena* arena;
    bool new_it, stay_dirty, prepare_failed;

    size_t tree_size;
    ntg_object** tree_pre; // May be NULL if alloc fails
    ntg_object** tree_post; // May be NULL if alloc fails
};

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static bool layout_layer(ntg_scene* scene, ntg_object* root, sarena* arena);

static inline void prepare_phase(ntg_object* root, struct layout_data* lay_data);
static inline void finalize_phase(ntg_object* root, struct layout_data* lay_data);
static inline void hmeasure_phase(ntg_object* root, struct layout_data* lay_data);
static inline void hconstrain_phase(ntg_object* root, struct layout_data* lay_data);
static inline void vmeasure_phase(ntg_object* root, struct layout_data* lay_data);
static inline void vconstrain_phase(ntg_object* root, struct layout_data* lay_data);
static inline void arrange_phase(ntg_object* root, struct layout_data* lay_data);
static inline void draw_phase(ntg_object* root, struct layout_data* lay_data);

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

int ntg_scene_init(
        ntg_scene* scene,
        const struct ntg_fcs_scope_keys* init_scope_keybinds,
        unsigned int max_it)
{
    return ntg_scene_init_inherit(
            scene,
            &NTG_SCENE_VTABLE,
            &NTG_TYPE_SCENE,
            init_scope_keybinds,
            max_it);
}

int ntg_scene_deinit(ntg_scene* scene)
{
    if(!scene) return NTG_ERR_INV_ARG;

    if(scene->ro.stage)
        ntg_stage_set_scene(scene->ro.stage, NULL);

    if(scene->ro.fm)
    {
        ntg__fcs_manager_deinit(scene->ro.fm);
        free(scene->ro.fm);
    }

    ntg_objptr_vec_deinit(&scene->ro.roots);

    ntg_entity_zero(scene);
    ntg_entity_deinit(ntg_ent(scene));

    return 0;
}

void ntg_scene_deinit_void(void* _scene)
{
    if(!_scene) return;

    ntg_scene_deinit(_scene);
}

int ntg_scene_mark_dirty(ntg_scene* scene)
{
    if(!scene) return NTG_ERR_INV_ARG;

    scene->ro.dirty = true;

    if(scene->ro.stage)
    {
        ntg_stage_mark_dirty(scene->ro.stage);
    }

    return 0;
}

/* ------------------------------------------------------ */
/* GENERAL */
/* ------------------------------------------------------ */

size_t ntg_scene_collect_layers_by_z(ntg_scene* scene, ntg_object** out_buff, size_t cap)
{
    if(!scene) return 0;

    size_t sum = 0;
    size_t new_cap = cap;

    const struct ntg_objptr_vec* roots = &scene->ro.roots;
    if(roots->size == 0)
        return 0;

    size_t i;
    size_t it_tree_count;
    for(i = 0; i < roots->size; i++)
    {
        it_tree_count = ntg_object_graph_collect_roots_pre(
                roots->data[i],
                out_buff ? out_buff + sum : NULL,
                new_cap);

        sum += it_tree_count;
        new_cap = _sub2_size(new_cap, it_tree_count);
    }

    if(out_buff)
    {
        size_t counted = _min2_size(cap, sum);
        ntg_object_sort_by_z(out_buff, counted);
    }

    return sum;
}

/* ------------------------------------------------------ */

int ntg_scene_hit_test(
        ntg_scene* scene,
        struct ntg_xy pos,
        struct ntg_scene_hit_res* out_res)
{
    struct ntg_scene_hit_res out = {0};
    ntg_set_out(out_res, out);

    if(!scene)
        return NTG_ERR_INV_ARG;

    size_t layer_count = scene->ro.tree_count;
    if(layer_count == 0) return 0;

    ntg_object** layers = malloc(layer_count * sizeof(ntg_object*));
    if(!layers)
        return NTG_ERR_ALLOC_FAIL;

    ntg_scene_collect_layers_by_z(scene, layers, layer_count);

    size_t i = layer_count;
    struct ntg_dxy it_adj_pos_dxy;
    struct ntg_xy it_adj_pos;
    // struct ntg_xy _out_object_pos;
    // enum ntg_object_hit_result _hit;
    struct ntg_object_hit_res it_res = {0};
    while(i > 0)
    {
        i--;
        it_adj_pos_dxy = ntg_object_map_from_scene(layers[i], ntg_dxy_from_xy(pos));

        if((it_adj_pos_dxy.x < 0) || (it_adj_pos_dxy.y < 0))
            continue;

        it_adj_pos = ntg_xy_from_dxy(it_adj_pos_dxy);

        it_res = ntg_object_hit_test(layers[i], it_adj_pos);
        if(it_res.object) break;
    }

    free(layers);

    out.res = it_res;
    ntg_set_out(out_res, out);

    return 0;
}

/*
int ntg_scene_set_root(ntg_scene* scene, ntg_object* root)
{
    if(!scene)
        return NTG_ERR_INV_ARG;

    ntg_object* old_root = scene->ro.root;

    if(old_root) 
    {
        ntg__object_root_set_scene(old_root, NULL);
        
        ntg__scene_rm_object_tree(scene, old_root);
    }

    if(root)
    {
        if(ntg_object_is_true_root(root)) 
        {
            ntg_scene* scene = ntg_object_get_scene_(root);
            if(scene)
            {
                ntg_scene_set_root(scene, NULL);
            }
        }
        else if(ntg_object_is_root(root)) 
        {
            ntg_object_unanchor(root);
        }

        ntg__scene_add_object_tree(scene, root);
        ntg__object_root_set_scene(root, scene);
    }

    scene->ro.root = root;
        
    if(old_root)
        ntg__scene_on_rm_object_tree(scene, old_root);

    if(root)
        ntg__scene_on_add_object_tree(scene, root);

    return 0;
}
*/

/* ------------------------------------------------------ */

int ntg_scene_add_root(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object)
        return NTG_ERR_INV_ARG;

    // If already root, success
    if(ntg_objptr_vec_exists(&scene->ro.roots, object))
        return 0;

    int status;

    status = ntg_object_remove_from_scene(object);
    switch(status)
    {
        case 0: break;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    status = ntg_objptr_vec_pushb(&scene->ro.roots, object);
    switch(status)
    {
        case 0: break;
        case GENC_ERR_ALLOC_FAIL:
            return NTG_ERR_ALLOC_FAIL;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    ntg__scene_add_object_tree(scene, object);
    ntg__scene_on_add_object_tree(scene, object);

    ntg__object_root_set_scene(object, scene);

    return 0;
}

int ntg_scene_rm_root(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object)
        return NTG_ERR_INV_ARG;

    // If not root, success
    if(!ntg_objptr_vec_exists(&scene->ro.roots, object))
        return 0;

    int status = ntg_objptr_vec_rm(&scene->ro.roots, object);
    switch(status)
    {
        case 0: break;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    ntg__scene_rm_object_tree(scene, object);
    ntg__scene_on_rm_object_tree(scene, object);

    ntg__object_root_set_scene(object, NULL);

    return 0;
}

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_scene_feed_key(ntg_scene* scene, nt_key key)
{
    if(!scene) return false;
    if(nt_key_are_eql(key, NT_KEY_ZERO)) return false;

    bool handled = false;

    if(ntg_scn_vtbl(scene) && ntg_scn_vtbl(scene)->handle_key_fn)
        handled = ntg_scn_vtbl(scene)->handle_key_fn(scene, key);

    struct ntg_event_scene_key_dt event_dt = { .key = key };
    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_KEY, &event_dt);

    return handled;
}

/* ------------------------------------------------------ */

bool ntg_scene_feed_mouse(ntg_scene* scene, nt_mouse mouse)
{
    if(!scene) return false;
    if(nt_mouse_are_eql(mouse, NT_MOUSE_ZERO)) return false;

    bool handled = false;

    if(ntg_scn_vtbl(scene) && ntg_scn_vtbl(scene)->handle_mouse_fn)
        handled = ntg_scn_vtbl(scene)->handle_mouse_fn(scene, mouse);

    struct ntg_event_scene_mouse_dt event_dt = { .mouse = mouse };
    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_MOUSE, &event_dt);

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

int ntg_scene_init_inherit(
        ntg_scene* scene,
        const struct ntg_scene_vtable* vtable,
        const ntg_type* type,
        const struct ntg_fcs_scope_keys* init_scope_keybinds,
        unsigned int max_it)
{
    if(!scene || !vtable || !type || !max_it)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_SCENE))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_entity_init_inherit(ntg_ent(scene), &vtable->base, type);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_entity_zero(scene);

    scene->ro.fm = malloc(sizeof(ntg_fcs_manager));
    if(!scene->ro.fm)
    {
        ntg_entity_deinit(ntg_ent(scene));
        return NTG_ERR_ALLOC_FAIL;
    }

    status = ntg__fcs_manager_init(scene->ro.fm, scene, init_scope_keybinds);
    if(status != 0)
    {
        free(scene->ro.fm);
        ntg_entity_zero(scene);
        ntg_entity_deinit(ntg_ent(scene));
        return status;
    }

    scene->priv.max_it = max_it;

    return 0;
}

/* ------------------------------------------------------ */
/* IMPLEMENT */
/* ------------------------------------------------------ */

const struct ntg_scene_vtable NTG_SCENE_VTABLE = {
    .base.deinit_fn = ntg_scene_deinit_fn,
    .handle_key_fn = ntg_scene_dispatch_key_fn,
    .handle_mouse_fn = ntg_scene_dispatch_mouse_fn
};

bool ntg_scene_dispatch_key_fn(ntg_scene* scene, nt_key key)
{
    if(!scene) return false;

    return ntg_fcs_manager_feed_key(scene->ro.fm, key);
}

bool ntg_scene_dispatch_mouse_fn(ntg_scene* scene, nt_mouse mouse)
{
    if(!scene) return false;

    return ntg_fcs_manager_feed_mouse(scene->ro.fm, mouse);
}

void ntg_scene_deinit_fn(ntg_entity* _scene)
{
    ntg_scene_deinit(ntg_scn(_scene));
}


/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg__scene_set_size(ntg_scene* scene, struct ntg_xy size)
{
    if(!scene)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    struct ntg_xy old_size = scene->ro.size;

    if(ntg_xy_are_eql(old_size, size))
        return 0;

    scene->ro.size = size;
    ntg_scene_mark_dirty(scene);
    
    struct ntg_event_scene_szchg_dt event_dt = {
        .old_x = old_size.x,
        .old_y = old_size.y,
        .new_x = size.x,
        .new_y = size.y
    };
    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_SZCHG, &event_dt);

    return 0;
}

bool ntg__scene_layout(ntg_scene* scene, sarena* arena)
{
    if(!scene) return false;

    ntg_log_log("LAYOUT");

    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_LAYPRE, NULL);

    bool relayout = false;

    if(scene->ro.tree_count != 0)
    {
        /* Total tree count will suffice (it is >= each graph tree count) */
        size_t cap = scene->ro.tree_count;
        ntg_object** buff = sarena_calloc(arena, sizeof(ntg_object*) * cap);
        if(!buff)
        {
            relayout = true;
        }
        else
        {
            size_t i, j;
            ntg_object* it_root;
            size_t it_count;
            for(i = 0; i < scene->ro.roots.size; i++)
            {
                it_root = scene->ro.roots.data[i];

                it_count = ntg_object_graph_collect_roots_pre(it_root, buff, cap);

                for(j = 0; j < it_count; j++)
                {
                    bool layer_relayout = layout_layer(scene, buff[j], arena);
                    relayout = relayout || layer_relayout;
                }
            }
        }
    }

    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_LAYPOST, NULL);

    return relayout;
}

void ntg__scene_clean(ntg_scene* scene)
{
    if(!scene) return;

    scene->ro.dirty = false;
}

void ntg__scene_set_stage(ntg_scene* scene, ntg_stage* stage)
{
    if(!scene) return;

    if(scene->ro.stage == stage) return;

    scene->ro.stage = stage;
    if(stage)
        ntg_scene_mark_dirty(scene);

}

void ntg__scene_on_stage_enter(ntg_scene* scene, ntg_stage* stage)
{
    if(!scene) return;

    struct ntg_event_scene_enter_dt event_dt = { .stage = stage };
    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_ENTER, &event_dt);

}

void ntg__scene_on_stage_leave(ntg_scene* scene, ntg_stage* stage)
{
    if(!scene) return;

    struct ntg_event_scene_leave_dt event_dt = { .stage = stage };
    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_LEAVE, &event_dt);

}

void ntg__scene_add(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    ntg_object_mark_dirty(object, NTG_OBJECT_DIRTY_FULL);
    ntg_scene_mark_dirty(scene);

    scene->ro.object_count++;
    if(ntg_object_is_tree_root(object))
        scene->ro.tree_count++;

    ntg__object_scene_enter(object, scene);
}

void ntg__scene_rm(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    ntg_scene_mark_dirty(scene);

    _sub2_size(scene->ro.object_count, 1);
    if(ntg_object_is_tree_root(object))
        _sub2_size(scene->ro.tree_count, 1);

    ntg__fcs_manager_on_scene_object_rm(scene->ro.fm, object);
    ntg__object_scene_leave(object, scene);
}

void ntg__scene_on_add_object(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    struct ntg_event_scene_objadd_dt object_event_dt = { .object = object };
    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_OBJADD, &object_event_dt);
}

void ntg__scene_on_rm_object(ntg_scene* scene, ntg_object* object)
{
    if(!scene || !object) return;

    struct ntg_event_scene_objrm_dt object_event_dt = { .object = object };
    ntg_entity_event_raise(ntg_ent(scene), NTG_EVENT_SCENE_OBJRM, &object_event_dt);
}

void ntg__scene_add_object_tree(ntg_scene* scene, ntg_object* root)
{
    if(!scene || !root) return;

    ntg__scene_add(scene, root);

    size_t i;
    for(i = 0; i < root->ro.children.size; i++)
    {
        ntg_object* child = root->ro.children.data[i];
        ntg__scene_add_object_tree(scene, child);
    }

    for(i = 0; i < root->ro.anchored.size; i++)
    {
        ntg_object* layer = root->ro.anchored.data[i];
        ntg__scene_add_object_tree(scene, layer);
    }

}

void ntg__scene_rm_object_tree(ntg_scene* scene, ntg_object* root)
{
    if(!scene || !root) return;

    ntg__scene_rm(scene, root);

    size_t i;
    for(i = 0; i < root->ro.children.size; i++)
    {
        ntg_object* child = root->ro.children.data[i];
        ntg__scene_rm_object_tree(scene, child);
    }

    for(i = 0; i < root->ro.anchored.size; i++)
    {
        ntg_object* layer = root->ro.anchored.data[i];
        ntg__scene_rm_object_tree(scene, layer);
    }

}

void ntg__scene_on_add_object_tree(ntg_scene* scene, ntg_object* root)
{
    if(!scene || !root) return;

    ntg__scene_on_add_object(scene, root);

    size_t i;
    for(i = 0; i < root->ro.children.size; i++)
    {
        ntg_object* child = root->ro.children.data[i];
        ntg__scene_on_add_object_tree(scene, child);
    }

    for(i = 0; i < root->ro.anchored.size; i++)
    {
        ntg_object* layer = root->ro.anchored.data[i];
        ntg__scene_on_add_object_tree(scene, layer);
    }

}

void ntg__scene_on_rm_object_tree(ntg_scene* scene, ntg_object* root)
{
    if(!scene || !root) return;

    ntg__scene_on_rm_object(scene, root);

    size_t i;
    for(i = 0; i < root->ro.children.size; i++)
    {
        ntg_object* child = root->ro.children.data[i];
        ntg__scene_on_rm_object_tree(scene, child);
    }

    for(i = 0; i < root->ro.anchored.size; i++)
    {
        ntg_object* layer = root->ro.anchored.data[i];
        ntg__scene_on_rm_object_tree(scene, layer);
    }

}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static inline void init_layout_data(
        ntg_scene* scene,
        ntg_object* root,
        sarena* arena,
        struct layout_data* lay_data)
{
    lay_data->arena = arena;
    lay_data->scene = scene;
    lay_data->new_it = false;
    lay_data->stay_dirty = false;
    lay_data->prepare_failed = false;
    lay_data->tree_size = ntg_object_tree_collect_pre(root, NULL, SIZE_MAX);

    lay_data->tree_pre = sarena_malloc(arena, lay_data->tree_size * sizeof(ntg_object*));
    if(lay_data->tree_pre)
    {
        ntg_object_tree_collect_pre(root, lay_data->tree_pre, lay_data->tree_size);
    }

    lay_data->tree_post = sarena_malloc(arena, lay_data->tree_size * sizeof(ntg_object*));
    if(lay_data->tree_pre && lay_data->tree_post)
    {
        ntg_object_tree_collect_post(root, lay_data->tree_post, lay_data->tree_size);
    }
}

static inline void
get_root_constrain_ctx(
        ntg_scene* scene,
        ntg_object* root,
        struct ntg_anchor_constrain_ctx* out_ctx,
        enum ntg_orient orient)
{
    ntg_object* base = root->ro.base;

    if(base)
    {
        (*out_ctx) = (struct ntg_anchor_constrain_ctx) {
            .root = root,
            .base_min_size = ntg_xy_get(ntg_object_get_min_size(base), orient),
            .base_nat_size = ntg_xy_get(ntg_object_get_nat_size(base), orient),
            .base_max_size = ntg_xy_get(ntg_object_get_max_size(base), orient),
            .base_size = ntg_xy_get(ntg_object_get_size(base), orient),
            .base_pos = ntg_xy_get(ntg_object_get_abs_pos(base), orient)
        };
    }
    else
    {
        size_t scene_size = ntg_xy_get(scene->ro.size, orient);

        (*out_ctx) = (struct ntg_anchor_constrain_ctx) {
            .root = root,
            .base_min_size = scene_size,
            .base_nat_size = scene_size,
            .base_max_size = scene_size,
            .base_size = scene_size,
            .base_pos = 0
        };
    }
}

static inline void
get_root_arrange_ctx(
        ntg_scene* scene,
        ntg_object* root,
        struct ntg_anchor_arrange_ctx* out_ctx)
{
    ntg_object* base = root->ro.base;

    if(base)
    {
        (*out_ctx) = (struct ntg_anchor_arrange_ctx) {
            .root = root,
            .size = ntg_object_get_size(root),
            .base_size = ntg_object_get_size(base),
            .base_pos = ntg_object_get_abs_pos(base)
        };
    }
    else
    {
        (*out_ctx) = (struct ntg_anchor_arrange_ctx) {
            .root = root,
            .size = ntg_object_get_size(root),
            .base_size = scene->ro.size,
            .base_pos = ntg_xy(0, 0)
        };
    }
}

static bool 
layout_layer(ntg_scene* scene, ntg_object* root, sarena* arena)
{
    if(!root) return false;

    struct layout_data layout_data = {0};
    init_layout_data(scene, root, arena, &layout_data);

    prepare_phase(root, &layout_data);

    size_t it_counter = 0;

    do
    {
        layout_data.stay_dirty = false;
        layout_data.new_it = false;

        hmeasure_phase(root, &layout_data);
        hconstrain_phase(root, &layout_data);
        vmeasure_phase(root, &layout_data);
        vconstrain_phase(root, &layout_data);
        arrange_phase(root, &layout_data);

        ++it_counter;
    }
    while((layout_data.new_it) && (it_counter < scene->priv.max_it));

    draw_phase(root, &layout_data);
    finalize_phase(root, &layout_data);

    ntg_log_log("IT: %d", it_counter);

    return (layout_data.prepare_failed || layout_data.new_it ||
            layout_data.stay_dirty);
}

static uint32_t masks[] = {
        0x01, // HM
        0x03, // HC
        0x07, // VM
        0x0F, // VC
        0x1F, // A
};

/* Don't require new iteration if set dirty phase is going to happen in this it. */
static inline bool new_it(uint32_t dirty_flags, unsigned int curr_phase)
{
    if(curr_phase > (sizeof(masks) / sizeof(uint32_t)))
        return false;

    return ((dirty_flags & masks[curr_phase]) != 0);
}

static inline void prepare_object(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->ro.dirty & NTG_OBJECT_DIRTY_PREPARE)
    {
        // ntg_log_log("NTG_SCENE | P | %p", object);

        int _status = ntg__object_layout_prepare(object, arena);
        if(_status)
            layout_data->prepare_failed = true;
        else
            ntg__object_clean(object, NTG_OBJECT_DIRTY_PREPARE);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | P SKIPPED | %p", object);
    }
}

static inline void hmeasure_object(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->ro.dirty & NTG_OBJECT_DIRTY_HMEASURE)
    {
        // ntg_log_log("NTG_SCENE | HM | %p", object);

        uint32_t _relayout = 0;
        int _status = ntg__object_hmeasure(object, arena, &_relayout);
        if(_status)
            layout_data->stay_dirty = true;
        else
            ntg__object_clean(object, NTG_OBJECT_DIRTY_HMEASURE);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 0);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | HM SKIPPED | %p", object);
    }
}

static inline void hconstrain_object(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->ro.dirty & NTG_OBJECT_DIRTY_HCONSTRAIN)
    {
        // ntg_log_log("NTG_SCENE | HC | %p", object);

        uint32_t _relayout = 0;
        int _status = ntg__object_hconstrain(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__object_clean(object, NTG_OBJECT_DIRTY_HCONSTRAIN);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 1);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | HC SKIPPED | %p", object);
    }
}

static inline void vmeasure_object(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->ro.dirty & NTG_OBJECT_DIRTY_VMEASURE)
    {
        // ntg_log_log("NTG_SCENE | VM | %p", object);

        uint32_t _relayout = 0;
        int _status = ntg__object_vmeasure(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__object_clean(object, NTG_OBJECT_DIRTY_VMEASURE);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 2);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | VM SKIPPED | %p", object);
    }
}

static inline void vconstrain_object(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->ro.dirty & NTG_OBJECT_DIRTY_VCONSTRAIN)
    {
        // ntg_log_log("NTG_SCENE | VC | %p", object);

        uint32_t _relayout = 0;
        int _status = ntg__object_vconstrain(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__object_clean(object, NTG_OBJECT_DIRTY_VCONSTRAIN);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 3);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | VC SKIPPED | %p", object);
    }
}

static inline void arrange_object(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->ro.dirty & NTG_OBJECT_DIRTY_ARRANGE)
    {
        // ntg_log_log("NTG_SCENE | A | %p", object);

        uint32_t _relayout = 0;
        int _status = ntg__object_arrange(object, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__object_clean(object, NTG_OBJECT_DIRTY_ARRANGE);

        if(_relayout)
            ntg_object_mark_dirty(object, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 4);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | A SKIPPED | %p", object);
    }
}

static inline void draw_object(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(object->ro.dirty & NTG_OBJECT_DIRTY_DRAW)
    {
        // ntg_log_log("NTG_SCENE | D | %p", object);

        int _status = ntg__object_draw(object, arena);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__object_clean(object, NTG_OBJECT_DIRTY_DRAW);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | D SKIPPED | %p", object);
    }
}

static inline void finalize_object(ntg_object* object, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    ntg__object_layout_finalize(object, arena);
}

NTG_OBJECT_TREE_DEF_TRAVERSE_PRE(prepare_tree, prepare_object)
NTG_OBJECT_TREE_DEF_TRAVERSE_POST(hmeasure_tree, hmeasure_object)
NTG_OBJECT_TREE_DEF_TRAVERSE_PRE(hconstrain_tree, hconstrain_object)
NTG_OBJECT_TREE_DEF_TRAVERSE_POST(vmeasure_tree, vmeasure_object)
NTG_OBJECT_TREE_DEF_TRAVERSE_PRE(vconstrain_tree, vconstrain_object)
NTG_OBJECT_TREE_DEF_TRAVERSE_PRE(arrange_tree, arrange_object)
NTG_OBJECT_TREE_DEF_TRAVERSE_PRE(draw_tree, draw_object)
NTG_OBJECT_TREE_DEF_TRAVERSE_PRE(finalize_tree, finalize_object)

static inline void prepare_phase(ntg_object* root, struct layout_data* lay_data)
{
    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            prepare_object(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        prepare_tree(root, lay_data);
    }
}

static inline void hmeasure_phase(ntg_object* root, struct layout_data* lay_data)
{
    if(lay_data->tree_post)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            hmeasure_object(lay_data->tree_post[i], lay_data);
    }
    else
    {
        hmeasure_tree(root, lay_data);
    }
}

static inline void hconstrain_phase(ntg_object* root, struct layout_data* lay_data)
{
    const struct ntg_anchor_policy* policy = root->ro.anchor_policy;
    ntg_scene* scene = lay_data->scene;
    sarena* arena = lay_data->arena;

    /* Anchor */

    struct ntg_anchor_constrain_ctx ctx;
    get_root_constrain_ctx(scene, root, &ctx, NTG_ORIENT_H);

    size_t it_hsize = 0;
    if(policy)
    {
        it_hsize = ntg__anchor_policy_hconstrain(policy, &ctx, arena);
        it_hsize = _clamp_size(0, it_hsize, scene->ro.size.x);
    }
    else
        it_hsize = ctx.base_size;

    if(root->ro.size.x != it_hsize)
        ntg__object_root_set_hsize(root, it_hsize);

    /* Tree */

    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            hconstrain_object(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        hconstrain_tree(root, lay_data);
    }
}

static inline void vmeasure_phase(ntg_object* root, struct layout_data* lay_data)
{
    if(lay_data->tree_post)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            vmeasure_object(lay_data->tree_post[i], lay_data);
    }
    else
    {
        vmeasure_tree(root, lay_data);
    }
}

static inline void vconstrain_phase(ntg_object* root, struct layout_data* lay_data)
{
    const struct ntg_anchor_policy* policy = root->ro.anchor_policy;
    ntg_scene* scene = lay_data->scene;
    sarena* arena = lay_data->arena;

    /* Anchor */

    struct ntg_anchor_constrain_ctx ctx;
    get_root_constrain_ctx(scene, root, &ctx, NTG_ORIENT_V);

    size_t it_vsize = 0;
    get_root_constrain_ctx(scene, root, &ctx, NTG_ORIENT_V);
    if(policy)
    {
        it_vsize = ntg__anchor_policy_vconstrain(policy, &ctx, arena);
        it_vsize = _clamp_size(0, it_vsize, scene->ro.size.y);
    }
    else
        it_vsize = ctx.base_size;

    if(root->ro.size.y != it_vsize)
        ntg__object_root_set_vsize(root, it_vsize);

    /* Tree */

    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            vconstrain_object(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        vconstrain_tree(root, lay_data);
    }
}

static inline void arrange_phase(ntg_object* root, struct layout_data* lay_data)
{
    const struct ntg_anchor_policy* policy = root->ro.anchor_policy;
    ntg_scene* scene = lay_data->scene;
    sarena* arena = lay_data->arena;
    struct ntg_xy size = root->ro.size;

    /* Anchor */

    struct ntg_anchor_arrange_ctx ctx;
    get_root_arrange_ctx(scene, root, &ctx);
    struct ntg_xy pos;
    if(policy)
    {
        pos = ntg__anchor_policy_arrange(policy, &ctx, arena);
        pos = ntg_xy_pos_clamp(pos, size, scene->ro.size);
    }
    else
        pos = ctx.base_pos;

    pos.x -= _sub2_size(pos.x + size.x, scene->ro.size.x);
    pos.y -= _sub2_size(pos.y + size.y, scene->ro.size.y);

    if(!ntg_xy_are_eql(root->ro.pos, pos))
        ntg__object_root_set_pos(root, pos);

    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            arrange_object(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        arrange_tree(root, lay_data);
    }
}

static inline void draw_phase(ntg_object* root, struct layout_data* lay_data)
{
    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            draw_object(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        draw_tree(root, lay_data);
    }
}

static inline void finalize_phase(ntg_object* root, struct layout_data* lay_data)
{
    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            finalize_object(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        finalize_tree(root, lay_data);
    }
}
