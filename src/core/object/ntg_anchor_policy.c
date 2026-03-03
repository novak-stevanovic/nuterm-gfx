#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

struct ntg_float_policy_dt
{
    ntg_object* anchor;
    struct ntg_float_policy_opts opts;
};

struct ntg_sidefloat_policy_dt
{
    ntg_object* anchor;
    struct ntg_sidefloat_policy_opts opts;
};

static size_t root_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy root_arrange_fn(
        const void* data,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

static size_t float_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy float_arrange_fn(
        const void* _data,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

static size_t sidefloat_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy sidefloat_arrange_fn(
        const void* _data,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

const struct ntg_anchor_policy NTG_ATTACH_POLICY_ROOT = {
    .data = NULL,
    .constrain_fn = root_constrain_fn,
    .arrange_fn = root_arrange_fn
};

const struct ntg_anchor_policy* ntg_anchor_policy_root()
{
    return &NTG_ATTACH_POLICY_ROOT;
}

struct ntg_float_policy_opts ntg_float_policy_opts_def()
{
    return (struct ntg_float_policy_opts) {
        .enable = NTG_FLOAT_POLICY_ENABLE_MIN,
        .shrink = ntg_insets(0, 0, 0, 0),
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1
    };
}

struct ntg_sidefloat_policy_opts ntg_sidefloat_policy_opts_def()
{
    return (struct ntg_sidefloat_policy_opts) {
        .align = NTG_ALIGN_1,
        .orient = NTG_SIDEFLOAT_POLICY_ORIENT_N,
        .thresh = NTG_SIDEFLOAT_POLICY_THRESH_MIN,
        .enable = NTG_SIDEFLOAT_POLICY_ENABLE_STATIC,
        .size_cap = NTG_SIDEFLOAT_POLICY_SZCAP_NONE
    };
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_anchor_policy_deinit(struct ntg_anchor_policy* policy)
{
    if(!policy) return;

    if(policy->free_fn)
        policy->free_fn(policy->data);

    (*policy) = (struct ntg_anchor_policy) {0};
}

void ntg_anchor_policy_deinit_(void* _policy)
{
    ntg_anchor_policy_deinit(_policy);
}

void ntg_anchor_policy_init_float(
        struct ntg_anchor_policy* policy,
        ntg_object* anchor,
        const struct ntg_float_policy_opts* opts)
{
    if(!policy) return;
    assert(anchor);
    
    struct ntg_float_policy_dt* data = malloc(sizeof(*data));
    assert(data);

    (*data) = (struct ntg_float_policy_dt) {
        .anchor = anchor,
        .opts = (opts ? (*opts) : ntg_float_policy_opts_def())
    };

    (*policy) = (struct ntg_anchor_policy) {
        .constrain_fn = float_constrain_fn,
        .arrange_fn = float_arrange_fn,
        .data = data,
        .free_fn = free
    };
}

void ntg_anchor_policy_init_sidefloat(
        struct ntg_anchor_policy* policy,
        ntg_object* anchor,
        const struct ntg_sidefloat_policy_opts* opts)
{
    if(!policy) return;
    assert(anchor);
    
    struct ntg_sidefloat_policy_dt* data = malloc(sizeof(*data));
    assert(data);

    (*data) = (struct ntg_sidefloat_policy_dt) {
        .anchor = anchor,
        .opts = (opts ? (*opts) : ntg_sidefloat_policy_opts_def())
    };

    (*policy) = (struct ntg_anchor_policy) {
        .constrain_fn = sidefloat_constrain_fn,
        .arrange_fn = sidefloat_arrange_fn,
        .data = data,
        .free_fn = free
    };
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static size_t root_constrain_fn(
        const void* data,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    const ntg_scene* scene = ntg_object_get_scene(ctx->root);
    if(!scene) return 0;

    return ntg_xy_get(scene->_size, orient);
}

static struct ntg_xy root_arrange_fn(
        const void* data,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    return ntg_xy(0, 0);
}

static size_t float_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    const struct ntg_float_policy_dt* data = _data;

    const ntg_object* base = ctx->base;
    const ntg_object* root = ctx->root;
    const ntg_object* anchor = data->anchor;

    if(!base || !ntg_object_is_descendant_eq(base, data->anchor))
        return 0;

    struct ntg_object_measure root_measure, anchor_measure;
    root_measure = ntg_object_get_measure(root, orient);
    anchor_measure = ntg_object_get_measure(anchor, orient);

    size_t anchor_size = ntg_xy_get(anchor->_size, orient);
    size_t shrink = ntg_insets_sum(data->opts.shrink, orient);
    size_t size = _min2_size(_ssub_size(anchor_size, shrink), root_measure.nat_size);

    size_t thresh;
    switch(data->opts.enable)
    {
        case NTG_FLOAT_POLICY_ENABLE_MIN:
            thresh = root_measure.min_size;
            break;
        case NTG_FLOAT_POLICY_ENABLE_BASE_MIN:
            thresh = anchor_measure.min_size;
            break;
        case NTG_FLOAT_POLICY_ENABLE_NAT:
            thresh = root_measure.nat_size;
            break;
        case NTG_FLOAT_POLICY_ENABLE_BASE_NAT:
            thresh = anchor_measure.nat_size;
            break;
        case NTG_FLOAT_POLICY_ENABLE_ALWAYS:
            thresh = 0;
            break;
    }

    return (size >= thresh) ? size : 0;
}

static struct ntg_xy float_arrange_fn(
        const void* _data,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    const struct ntg_float_policy_dt* data = _data;

    const ntg_object* base = ctx->base;
    const ntg_object* anchor = data->anchor;

    if(!base || !ntg_object_is_descendant_eq(base, anchor))
        return ntg_xy(0, 0);

    if(ntg_xy_size_is_zero(ctx->size))
        return ntg_xy(0, 0);

    struct ntg_xy shrink = ntg_xy(
        ntg_insets_hsum(data->opts.shrink),
        ntg_insets_vsum(data->opts.shrink));
    struct ntg_xy base_size = ntg_xy_sub(anchor->_size, shrink);

    struct ntg_xy align_offset = ntg_xy(
            ntg_align_offset(ctx->size.x, base_size.x, data->opts.prim_align),
            ntg_align_offset(ctx->size.y, base_size.y, data->opts.sec_align));

    align_offset.x += data->opts.shrink.w;
    align_offset.y += data->opts.shrink.n;

    struct ntg_xy
    pos = ntg_xy_from_dxy(ntg_object_map_to_scene(anchor, ntg_dxy(0, 0)));

    return ntg_xy_add(pos, align_offset);
}

static size_t sidefloat_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    const struct ntg_sidefloat_policy_dt* data = _data;
    const ntg_object* root = ctx->root;
    const ntg_object* base = ctx->root;
    const ntg_object* anchor = data->anchor;

    if(!ntg_object_is_descendant_eq(base, anchor))
        return 0;

    struct ntg_xy _anchor_pos;
    _anchor_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(anchor, ntg_dxy(0, 0)));
    size_t anchor_pos = ntg_xy_get(_anchor_pos, orient);

    struct ntg_object_measure root_measure;
    root_measure = ntg_object_get_measure(root, orient);

    const ntg_scene* scene = ntg_object_get_scene(ctx->root);
    size_t scene_size = (scene ? ntg_xy_get(scene->_size, orient) : 0);

    size_t anchor_size = ntg_xy_get(anchor->_size, orient);
    if(anchor_size == 0) return 0;

    size_t available_space;
    bool capped = (data->opts.size_cap == NTG_SIDEFLOAT_POLICY_SZCAP_ANCHOR);
    switch(data->opts.orient)
    {
        case NTG_SIDEFLOAT_POLICY_ORIENT_N:
            available_space = (orient == NTG_ORIENT_H) ?
                (capped ? anchor_size : scene_size) :
                anchor_pos;
            break;

        case NTG_SIDEFLOAT_POLICY_ORIENT_E:
            available_space = (orient == NTG_ORIENT_V) ?
                (capped ? anchor_size : scene_size) :
                _ssub_size(scene_size, anchor_size + anchor_pos);

            break;

        case NTG_SIDEFLOAT_POLICY_ORIENT_S:
            available_space = (orient == NTG_ORIENT_H) ?
                (capped ? anchor_size : scene_size) :
                _ssub_size(scene_size, anchor_size + anchor_pos);
            break;

        case NTG_SIDEFLOAT_POLICY_ORIENT_W:
            available_space = (orient == NTG_ORIENT_V) ?
                (capped ? anchor_size : scene_size) :
                anchor_pos;
            break;

        default:
            assert(0);
    }

    size_t thresh_size;
    switch(data->opts.thresh)
    {
        case NTG_SIDEFLOAT_POLICY_THRESH_MIN:
            thresh_size = root_measure.min_size;
            break;
        case NTG_SIDEFLOAT_POLICY_THRESH_NAT:
            thresh_size = root_measure.nat_size;
            break;
        case NTG_SIDEFLOAT_POLICY_THRESH_ALWAYS:
            thresh_size = 0;
            break;
        default:
            assert(0);
    }

    if(available_space < thresh_size)
        return 0;
    else
        return _min2_size(available_space, root_measure.nat_size);
}

static struct ntg_xy sidefloat_arrange_fn(
        const void* _data,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    const struct ntg_sidefloat_policy_dt* data = _data;
    const ntg_object* root = ctx->root;
    const ntg_object* base = ctx->root;
    const ntg_object* anchor = data->anchor;

    if(!ntg_object_is_descendant_eq(base, anchor))
        return ntg_xy(0, 0);

    struct ntg_xy anchor_pos;
    anchor_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(anchor, ntg_dxy(0, 0)));

    const ntg_scene* scene = ntg_object_get_scene(ctx->root);
    struct ntg_xy scene_size = (scene ? scene->_size : ntg_xy(0, 0));

    size_t align_total_size, align_content_size;

    ntg_orient align_orient = (
            ((data->opts.orient == NTG_SIDEFLOAT_POLICY_ORIENT_N) ||
                (data->opts.orient == NTG_SIDEFLOAT_POLICY_ORIENT_S)) ?
            NTG_ORIENT_H : NTG_ORIENT_V);

    bool capped = (data->opts.size_cap == NTG_SIDEFLOAT_POLICY_SZCAP_ANCHOR);

    align_total_size = (
            capped ?
            ntg_xy_get(anchor->_size, align_orient) :
            ntg_xy_get(scene_size, align_orient));

    align_content_size = ntg_xy_get(root->_size, align_orient);

    size_t align_pos = ntg_align_offset(align_content_size,
            align_total_size, data->opts.align);

    struct ntg_xy pos = ntg_xy(0, 0);
    switch(data->opts.orient)
    {
        case NTG_SIDEFLOAT_POLICY_ORIENT_N:
            pos.x = (capped ? anchor_pos.x + align_pos : align_pos);
            pos.y = _ssub_size(anchor_pos.y, root->_size.y);
            break;
        case NTG_SIDEFLOAT_POLICY_ORIENT_E:
            pos.y = (capped ? anchor_pos.y + align_pos : align_pos);
            pos.x = anchor_pos.x + anchor->_size.x;
            break;
        case NTG_SIDEFLOAT_POLICY_ORIENT_S:
            pos.x = (capped ? anchor_pos.x + align_pos : align_pos);
            pos.y = anchor_pos.y + anchor->_size.y;
            break;
        case NTG_SIDEFLOAT_POLICY_ORIENT_W:
            pos.y = (capped ? anchor_pos.y + align_pos : align_pos);
            pos.x = _ssub_size(anchor_pos.x, root->_size.x);
            break;
    }

    return ntg_xy_clamp(ntg_xy(0, 0), pos, scene_size);
}
