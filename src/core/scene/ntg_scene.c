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
    ntg_widget** tree_pre; // May be NULL if alloc fails
    ntg_widget** tree_post; // May be NULL if alloc fails
};

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

static bool layout_layer(ntg_scene* scene, ntg_widget* root, sarena* arena);

static inline void prepare_phase(ntg_widget* root, struct layout_data* lay_data);
static inline void finalize_phase(ntg_widget* root, struct layout_data* lay_data);
static inline void hmeasure_phase(ntg_widget* root, struct layout_data* lay_data);
static inline void hconstrain_phase(ntg_widget* root, struct layout_data* lay_data);
static inline void vmeasure_phase(ntg_widget* root, struct layout_data* lay_data);
static inline void vconstrain_phase(ntg_widget* root, struct layout_data* lay_data);
static inline void arrange_phase(ntg_widget* root, struct layout_data* lay_data);
static inline void draw_phase(ntg_widget* root, struct layout_data* lay_data);

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

int ntg_scene_init(ntg_scene* scene, const struct ntg_scene_init_opts* opts)
{
    if(!scene) return NTG_ERR_INV_ARG;

    int status = ntg_scene_init_inherit(scene, &NTG_SCENE_VTABLE, &NTG_TYPE_SCENE, opts);
    NTG_POST_INHERIT_CHECK(status);

    return 0;
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

    ntg_widget_vec_deinit(&scene->ro.roots);

    ntg_object_zero(scene);
    ntg_object_deinit(ntg_obj(scene));

    return 0;
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

size_t ntg_scene_collect_layers_by_z(ntg_scene* scene, ntg_widget** out_buff, size_t cap)
{
    if(!scene) return 0;

    size_t sum = 0;
    size_t new_cap = cap;

    const struct ntg_widget_vec* roots = &scene->ro.roots;
    if(roots->size == 0)
        return 0;

    size_t i;
    size_t it_tree_count;
    for(i = 0; i < roots->size; i++)
    {
        it_tree_count = ntg_widget_graph_collect_roots_pre(
                roots->data[i],
                out_buff ? out_buff + sum : NULL,
                new_cap);

        sum += it_tree_count;
        new_cap = ntg_sub2_size(new_cap, it_tree_count);
    }

    if(out_buff)
    {
        size_t counted = ntg_min2_size(cap, sum);
        ntg_widget_sort_by_z(out_buff, counted);
    }

    return sum;
}

/* ------------------------------------------------------ */

int ntg_scene_hit_test(
        ntg_scene* scene,
        ntg_xy pos,
        struct ntg_scene_hit_res* out_res)
{
    struct ntg_scene_hit_res out = {0};
    ntg_set_out(out_res, out);

    if(!scene)
        return NTG_ERR_INV_ARG;

    size_t layer_count = scene->ro.tree_count;
    if(layer_count == 0) return 0;

    ntg_widget** layers = malloc(layer_count * sizeof(ntg_widget*));
    if(!layers)
        return NTG_ERR_ALLOC_FAIL;

    ntg_scene_collect_layers_by_z(scene, layers, layer_count);

    size_t i = layer_count;
    ntg_dxy it_adj_pos_dxy;
    ntg_xy it_adj_pos;
    // ntg_xy _out_widget_pos;
    // enum ntg_widget_hit_result _hit;
    struct ntg_widget_hit_res it_res = {0};
    while(i > 0)
    {
        i--;
        it_adj_pos_dxy = ntg_widget_map_from_scene(layers[i], ntg_dxy_from_xy(pos));

        if((it_adj_pos_dxy.ro.x < 0) || (it_adj_pos_dxy.ro.y < 0))
            continue;

        it_adj_pos = ntg_xy_from_dxy(it_adj_pos_dxy);

        it_res = ntg_widget_hit_test(layers[i], it_adj_pos);
        if(it_res.widget) break;
    }

    free(layers);

    out.res = it_res;
    ntg_set_out(out_res, out);

    return 0;
}

/* ------------------------------------------------------ */

int ntg_scene_add_root(ntg_scene* scene, ntg_widget* widget)
{
    if(!scene || !widget)
        return NTG_ERR_INV_ARG;

    // If already root, success
    if(ntg_widget_vec_exists(&scene->ro.roots, widget))
        return 0;

    int status;

    status = ntg_widget_remove_from_scene(widget);
    switch(status)
    {
        case 0: break;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    status = ntg_widget_vec_pushb(&scene->ro.roots, widget);
    switch(status)
    {
        case 0: break;
        case GENC_ERR_ALLOC_FAIL:
            return NTG_ERR_ALLOC_FAIL;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    ntg__scene_add_widget_tree(scene, widget);
    ntg__scene_on_add_widget_tree(scene, widget);

    ntg__widget_root_set_scene(widget, scene);

    return 0;
}

int ntg_scene_rm_root(ntg_scene* scene, ntg_widget* widget)
{
    if(!scene || !widget)
        return NTG_ERR_INV_ARG;

    // If not root, success
    if(!ntg_widget_vec_exists(&scene->ro.roots, widget))
        return 0;

    int status = ntg_widget_vec_rm(&scene->ro.roots, widget);
    switch(status)
    {
        case 0: break;
        default:
            return NTG_ERR_UNEXPECTED;
    }

    ntg__scene_rm_widget_tree(scene, widget);
    ntg__scene_on_rm_widget_tree(scene, widget);

    ntg__widget_root_set_scene(widget, NULL);

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
    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_KEY, &event_dt);

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
    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_MOUSE, &event_dt);

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
        const struct ntg_scene_init_opts* opts)
{
    if(!scene || !vtable || !type)
        return NTG_ERR_INV_ARG;

    struct ntg_scene_init_opts opts_final = NTG_SCENE_INIT_OPTS_ZERO;
    if(opts) opts_final = (*opts);
    if(opts_final.max_it == 0)
        opts_final.max_it = NTG_SCENE_MAX_IT_AUTO;

    if(!ntg_type_instanceof(type, &NTG_TYPE_SCENE))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_object_init_inherit(ntg_obj(scene), &vtable->base, type);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(scene);

    scene->ro.fm = malloc(sizeof(ntg_fcs_manager));
    if(!scene->ro.fm)
    {
        ntg_object_deinit(ntg_obj(scene));
        return NTG_ERR_ALLOC_FAIL;
    }

    status = ntg__fcs_manager_init(scene->ro.fm, scene, opts_final.init_keys);
    if(status != 0)
    {
        free(scene->ro.fm);
        ntg_object_zero(scene);
        ntg_object_deinit(ntg_obj(scene));
        return status;
    }

    scene->priv.max_it = opts_final.max_it;

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

void ntg_scene_deinit_fn(ntg_object* _scene)
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

int ntg__scene_set_size(ntg_scene* scene, ntg_xy size)
{
    if(!scene)
        return NTG_ERR_INV_ARG;

    if((size.ro.x > NTG_SIZE_MAX) || (size.ro.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    ntg_xy old_size = scene->ro.size;

    if(ntg_xy_are_eql(old_size, size))
        return 0;

    scene->ro.size = size;
    ntg_scene_mark_dirty(scene);
    
    struct ntg_event_scene_szchg_dt event_dt = {
        .old_x = old_size.ro.x,
        .old_y = old_size.ro.y,
        .new_x = size.ro.x,
        .new_y = size.ro.y
    };
    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_SZCHG, &event_dt);

    return 0;
}

bool ntg__scene_layout(ntg_scene* scene, sarena* arena)
{
    if(!scene) return false;

    ntg_log_log("LAYOUT");

    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_LAYPRE, NULL);

    bool relayout = false;

    if(scene->ro.tree_count != 0)
    {
        /* Total tree count will suffice (it is >= each graph tree count) */
        size_t cap = scene->ro.tree_count;
        ntg_widget** buff = sarena_calloc(arena, sizeof(ntg_widget*) * cap);
        if(!buff)
        {
            relayout = true;
        }
        else
        {
            size_t i, j;
            ntg_widget* it_root;
            size_t it_count;
            for(i = 0; i < scene->ro.roots.size; i++)
            {
                it_root = scene->ro.roots.data[i];

                it_count = ntg_widget_graph_collect_roots_pre(it_root, buff, cap);

                for(j = 0; j < it_count; j++)
                {
                    bool layer_relayout = layout_layer(scene, buff[j], arena);
                    relayout = relayout || layer_relayout;
                }
            }
        }
    }

    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_LAYPOST, NULL);

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
    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_ENTER, &event_dt);

}

void ntg__scene_on_stage_leave(ntg_scene* scene, ntg_stage* stage)
{
    if(!scene) return;

    struct ntg_event_scene_leave_dt event_dt = { .stage = stage };
    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_LEAVE, &event_dt);

}

void ntg__scene_add(ntg_scene* scene, ntg_widget* widget)
{
    if(!scene || !widget) return;

    ntg_widget_mark_dirty(widget, NTG_WIDGET_DIRTY_FULL);
    ntg_scene_mark_dirty(scene);

    scene->ro.object_count++;
    if(ntg_widget_is_tree_root(widget))
        scene->ro.tree_count++;

    ntg__widget_scene_enter(widget, scene);
}

void ntg__scene_rm(ntg_scene* scene, ntg_widget* widget)
{
    if(!scene || !widget) return;

    ntg_scene_mark_dirty(scene);

    scene->ro.object_count = ntg_sub2_size(scene->ro.object_count, 1);
    if(ntg_widget_is_tree_root(widget))
        scene->ro.tree_count = ntg_sub2_size(scene->ro.tree_count, 1);

    ntg__fcs_manager_on_scene_widget_rm(scene->ro.fm, widget);
    ntg__widget_scene_leave(widget, scene);
}

void ntg__scene_on_add_widget(ntg_scene* scene, ntg_widget* widget)
{
    if(!scene || !widget) return;

    struct ntg_event_scene_wgtadd_dt object_event_dt = { .widget = widget };
    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_WGTADD, &object_event_dt);
}

void ntg__scene_on_rm_widget(ntg_scene* scene, ntg_widget* widget)
{
    if(!scene || !widget) return;

    struct ntg_event_scene_wgtrm_dt object_event_dt = { .widget = widget };
    ntg_object_event_raise(ntg_obj(scene), NTG_EVENT_SCENE_WGTRM, &object_event_dt);
}

void ntg__scene_add_widget_tree(ntg_scene* scene, ntg_widget* root)
{
    if(!scene || !root) return;

    ntg__scene_add(scene, root);

    size_t i;
    for(i = 0; i < root->ro.children.size; i++)
    {
        ntg_widget* child = root->ro.children.data[i];
        ntg__scene_add_widget_tree(scene, child);
    }

    for(i = 0; i < root->ro.anchored.size; i++)
    {
        ntg_widget* layer = root->ro.anchored.data[i];
        ntg__scene_add_widget_tree(scene, layer);
    }

}

void ntg__scene_rm_widget_tree(ntg_scene* scene, ntg_widget* root)
{
    if(!scene || !root) return;

    ntg__scene_rm(scene, root);

    size_t i;
    for(i = 0; i < root->ro.children.size; i++)
    {
        ntg_widget* child = root->ro.children.data[i];
        ntg__scene_rm_widget_tree(scene, child);
    }

    for(i = 0; i < root->ro.anchored.size; i++)
    {
        ntg_widget* layer = root->ro.anchored.data[i];
        ntg__scene_rm_widget_tree(scene, layer);
    }

}

void ntg__scene_on_add_widget_tree(ntg_scene* scene, ntg_widget* root)
{
    if(!scene || !root) return;

    ntg__scene_on_add_widget(scene, root);

    size_t i;
    for(i = 0; i < root->ro.children.size; i++)
    {
        ntg_widget* child = root->ro.children.data[i];
        ntg__scene_on_add_widget_tree(scene, child);
    }

    for(i = 0; i < root->ro.anchored.size; i++)
    {
        ntg_widget* layer = root->ro.anchored.data[i];
        ntg__scene_on_add_widget_tree(scene, layer);
    }

}

void ntg__scene_on_rm_widget_tree(ntg_scene* scene, ntg_widget* root)
{
    if(!scene || !root) return;

    ntg__scene_on_rm_widget(scene, root);

    size_t i;
    for(i = 0; i < root->ro.children.size; i++)
    {
        ntg_widget* child = root->ro.children.data[i];
        ntg__scene_on_rm_widget_tree(scene, child);
    }

    for(i = 0; i < root->ro.anchored.size; i++)
    {
        ntg_widget* layer = root->ro.anchored.data[i];
        ntg__scene_on_rm_widget_tree(scene, layer);
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
        ntg_widget* root,
        sarena* arena,
        struct layout_data* lay_data)
{
    lay_data->arena = arena;
    lay_data->scene = scene;
    lay_data->new_it = false;
    lay_data->stay_dirty = false;
    lay_data->prepare_failed = false;
    lay_data->tree_size = ntg_widget_tree_collect_pre(root, NULL, SIZE_MAX);

    lay_data->tree_pre = sarena_malloc(arena, lay_data->tree_size * sizeof(ntg_widget*));
    if(lay_data->tree_pre)
    {
        ntg_widget_tree_collect_pre(root, lay_data->tree_pre, lay_data->tree_size);
    }

    lay_data->tree_post = sarena_malloc(arena, lay_data->tree_size * sizeof(ntg_widget*));
    if(lay_data->tree_pre && lay_data->tree_post)
    {
        ntg_widget_tree_collect_post(root, lay_data->tree_post, lay_data->tree_size);
    }
}

static inline void
get_root_constrain_ctx(
        ntg_scene* scene,
        ntg_widget* root,
        struct ntg_anchor_constrain_ctx* out_ctx,
        enum ntg_orient orient)
{
    ntg_widget* base = root->ro.base;

    if(base)
    {
        (*out_ctx) = (struct ntg_anchor_constrain_ctx) {
            .root = root,
            .base_min_size = ntg_xy_get(ntg_widget_get_min_size(base), orient),
            .base_nat_size = ntg_xy_get(ntg_widget_get_nat_size(base), orient),
            .base_max_size = ntg_xy_get(ntg_widget_get_max_size(base), orient),
            .base_size = ntg_xy_get(ntg_widget_get_size(base), orient),
            .base_pos = ntg_xy_get(ntg_widget_get_abs_pos(base), orient)
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
        ntg_widget* root,
        struct ntg_anchor_arrange_ctx* out_ctx)
{
    ntg_widget* base = root->ro.base;

    if(base)
    {
        (*out_ctx) = (struct ntg_anchor_arrange_ctx) {
            .root = root,
            .size = ntg_widget_get_size(root),
            .base_size = ntg_widget_get_size(base),
            .base_pos = ntg_widget_get_abs_pos(base)
        };
    }
    else
    {
        (*out_ctx) = (struct ntg_anchor_arrange_ctx) {
            .root = root,
            .size = ntg_widget_get_size(root),
            .base_size = scene->ro.size,
            .base_pos = ntg_xy_new(0, 0)
        };
    }
}

static bool 
layout_layer(ntg_scene* scene, ntg_widget* root, sarena* arena)
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

static inline void prepare_widget(ntg_widget* widget, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(widget->ro.dirty & NTG_WIDGET_DIRTY_PREPARE)
    {
        // ntg_log_log("NTG_SCENE | P | %p", widget);

        int _status = ntg__widget_layout_prepare(widget, arena);
        if(_status)
            layout_data->prepare_failed = true;
        else
            ntg__widget_clean(widget, NTG_WIDGET_DIRTY_PREPARE);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | P SKIPPED | %p", widget);
    }
}

static inline void hmeasure_widget(ntg_widget* widget, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(widget->ro.dirty & NTG_WIDGET_DIRTY_HMEASURE)
    {
        // ntg_log_log("NTG_SCENE | HM | %p", widget);

        uint32_t _relayout = 0;
        int _status = ntg__widget_hmeasure(widget, arena, &_relayout);
        if(_status)
            layout_data->stay_dirty = true;
        else
            ntg__widget_clean(widget, NTG_WIDGET_DIRTY_HMEASURE);

        if(_relayout)
            ntg_widget_mark_dirty(widget, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 0);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | HM SKIPPED | %p", widget);
    }
}

static inline void hconstrain_widget(ntg_widget* widget, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(widget->ro.dirty & NTG_WIDGET_DIRTY_HCONSTRAIN)
    {
        // ntg_log_log("NTG_SCENE | HC | %p", widget);

        uint32_t _relayout = 0;
        int _status = ntg__widget_hconstrain(widget, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__widget_clean(widget, NTG_WIDGET_DIRTY_HCONSTRAIN);

        if(_relayout)
            ntg_widget_mark_dirty(widget, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 1);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | HC SKIPPED | %p", widget);
    }
}

static inline void vmeasure_widget(ntg_widget* widget, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(widget->ro.dirty & NTG_WIDGET_DIRTY_VMEASURE)
    {
        // ntg_log_log("NTG_SCENE | VM | %p", widget);

        uint32_t _relayout = 0;
        int _status = ntg__widget_vmeasure(widget, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__widget_clean(widget, NTG_WIDGET_DIRTY_VMEASURE);

        if(_relayout)
            ntg_widget_mark_dirty(widget, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 2);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | VM SKIPPED | %p", widget);
    }
}

static inline void vconstrain_widget(ntg_widget* widget, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(widget->ro.dirty & NTG_WIDGET_DIRTY_VCONSTRAIN)
    {
        // ntg_log_log("NTG_SCENE | VC | %p", widget);

        uint32_t _relayout = 0;
        int _status = ntg__widget_vconstrain(widget, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__widget_clean(widget, NTG_WIDGET_DIRTY_VCONSTRAIN);

        if(_relayout)
            ntg_widget_mark_dirty(widget, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 3);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | VC SKIPPED | %p", widget);
    }
}

static inline void arrange_widget(ntg_widget* widget, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(widget->ro.dirty & NTG_WIDGET_DIRTY_ARRANGE)
    {
        // ntg_log_log("NTG_SCENE | A | %p", widget);

        uint32_t _relayout = 0;
        int _status = ntg__widget_arrange(widget, arena, &_relayout);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__widget_clean(widget, NTG_WIDGET_DIRTY_ARRANGE);

        if(_relayout)
            ntg_widget_mark_dirty(widget, _relayout);
        layout_data->new_it = layout_data->new_it || new_it(_relayout, 4);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | A SKIPPED | %p", widget);
    }
}

static inline void draw_widget(ntg_widget* widget, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    if(widget->ro.dirty & NTG_WIDGET_DIRTY_DRAW)
    {
        // ntg_log_log("NTG_SCENE | D | %p", widget);

        int _status = ntg__widget_draw(widget, arena);
        if(_status != 0)
            layout_data->stay_dirty = true;
        else
            ntg__widget_clean(widget, NTG_WIDGET_DIRTY_DRAW);
    }
    else
    {
        // ntg_log_log("NTG_SCENE | D SKIPPED | %p", widget);
    }
}

static inline void finalize_widget(ntg_widget* widget, void* _layout_data)
{
    struct layout_data* layout_data = _layout_data;
    sarena* arena = layout_data->arena; 

    ntg__widget_layout_finalize(widget, arena);

    ntg_log_log("NTG_SCENE: F | OBJ_NAME: %s | SIZE: (%d, %d) | POS: (%d, %d) | ABS_POS: (%d, %d)",
        ntg_obj_name(widget),
        widget->ro.size.ro.x, widget->ro.size.ro.y,
        widget->ro.pos.ro.x, widget->ro.pos.ro.y,
        ntg_widget_get_abs_pos(widget).ro.x, ntg_widget_get_abs_pos(widget).ro.y);
}

NTG_WIDGET_TREE_DEF_TRAVERSE_PRE(prepare_tree, prepare_widget)
NTG_WIDGET_TREE_DEF_TRAVERSE_POST(hmeasure_tree, hmeasure_widget)
NTG_WIDGET_TREE_DEF_TRAVERSE_PRE(hconstrain_tree, hconstrain_widget)
NTG_WIDGET_TREE_DEF_TRAVERSE_POST(vmeasure_tree, vmeasure_widget)
NTG_WIDGET_TREE_DEF_TRAVERSE_PRE(vconstrain_tree, vconstrain_widget)
NTG_WIDGET_TREE_DEF_TRAVERSE_PRE(arrange_tree, arrange_widget)
NTG_WIDGET_TREE_DEF_TRAVERSE_PRE(draw_tree, draw_widget)
NTG_WIDGET_TREE_DEF_TRAVERSE_PRE(finalize_tree, finalize_widget)

static inline void prepare_phase(ntg_widget* root, struct layout_data* lay_data)
{
    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            prepare_widget(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        prepare_tree(root, lay_data);
    }
}

static inline void hmeasure_phase(ntg_widget* root, struct layout_data* lay_data)
{
    if(lay_data->tree_post)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            hmeasure_widget(lay_data->tree_post[i], lay_data);
    }
    else
    {
        hmeasure_tree(root, lay_data);
    }
}

static inline void hconstrain_phase(ntg_widget* root, struct layout_data* lay_data)
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
        it_hsize = ntg_clamp_size(0, it_hsize, scene->ro.size.ro.x);
    }
    else
        it_hsize = ctx.base_size;

    if(root->ro.size.ro.x != it_hsize)
        ntg__widget_root_set_hsize(root, it_hsize);

    /* Tree */

    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            hconstrain_widget(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        hconstrain_tree(root, lay_data);
    }
}

static inline void vmeasure_phase(ntg_widget* root, struct layout_data* lay_data)
{
    if(lay_data->tree_post)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            vmeasure_widget(lay_data->tree_post[i], lay_data);
    }
    else
    {
        vmeasure_tree(root, lay_data);
    }
}

static inline void vconstrain_phase(ntg_widget* root, struct layout_data* lay_data)
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
        it_vsize = ntg_clamp_size(0, it_vsize, scene->ro.size.ro.y);
    }
    else
        it_vsize = ctx.base_size;

    if(root->ro.size.ro.y != it_vsize)
        ntg__widget_root_set_vsize(root, it_vsize);

    /* Tree */

    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            vconstrain_widget(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        vconstrain_tree(root, lay_data);
    }
}

static inline void arrange_phase(ntg_widget* root, struct layout_data* lay_data)
{
    const struct ntg_anchor_policy* policy = root->ro.anchor_policy;
    ntg_scene* scene = lay_data->scene;
    sarena* arena = lay_data->arena;
    ntg_xy size = root->ro.size;

    /* Anchor */

    struct ntg_anchor_arrange_ctx ctx;
    get_root_arrange_ctx(scene, root, &ctx);
    ntg_xy pos;
    if(policy)
    {
        pos = ntg__anchor_policy_arrange(policy, &ctx, arena);
        pos = ntg_xy_pos_clamp(pos, size, scene->ro.size);
    }
    else
        pos = ctx.base_pos;

    pos = ntg_xy_sub(pos, ntg_xy_new(
            ntg_sub2_size(pos.ro.x + size.ro.x, scene->ro.size.ro.x),
            ntg_sub2_size(pos.ro.y + size.ro.y, scene->ro.size.ro.y)));

    if(!ntg_xy_are_eql(root->ro.pos, pos))
        ntg__widget_root_set_pos(root, pos);

    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            arrange_widget(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        arrange_tree(root, lay_data);
    }
}

static inline void draw_phase(ntg_widget* root, struct layout_data* lay_data)
{
    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            draw_widget(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        draw_tree(root, lay_data);
    }
}

static inline void finalize_phase(ntg_widget* root, struct layout_data* lay_data)
{
    if(lay_data->tree_pre)
    {
        size_t i;
        for(i = 0; i < lay_data->tree_size; i++)
            finalize_widget(lay_data->tree_pre[i], lay_data);
    }
    else
    {
        finalize_tree(root, lay_data);
    }
}
