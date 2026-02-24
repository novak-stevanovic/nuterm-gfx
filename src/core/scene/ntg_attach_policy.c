#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

struct ntg_attach_policy_flt_dt
{
    ntg_object* anchor;
    struct ntg_attach_policy_flt_opts opts;
};

struct ntg_attach_policy_sflt_dt
{
    ntg_object* anchor;
    struct ntg_attach_policy_sflt_opts opts;
};

static size_t root_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_attach_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy root_arrange_fn(
        const void* data,
        const struct ntg_attach_arrange_ctx* ctx,
        sarena* arena);

static size_t flt_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_attach_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy flt_arrange_fn(
        const void* _data,
        const struct ntg_attach_arrange_ctx* ctx,
        sarena* arena);

static size_t sflt_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_attach_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy sflt_arrange_fn(
        const void* _data,
        const struct ntg_attach_arrange_ctx* ctx,
        sarena* arena);

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

const struct ntg_attach_policy NTG_ATTACH_POLICY_ROOT = {
    .data = NULL,
    .constrain_fn = root_constrain_fn,
    .arrange_fn = root_arrange_fn
};

const struct ntg_attach_policy* ntg_attach_policy_root()
{
    return &NTG_ATTACH_POLICY_ROOT;
}

struct ntg_attach_policy_flt_opts ntg_attach_policy_flt_opts_def()
{
    return (struct ntg_attach_policy_flt_opts) {
        .enable = NTG_ATTACH_POLICY_FLT_ENABLE_MIN,
        .shrink = ntg_insets(0, 0, 0, 0),
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1
    };
}

struct ntg_attach_policy_sflt_opts ntg_attach_policy_sflt_opts_def()
{
    return (struct ntg_attach_policy_sflt_opts) {
        .align = NTG_ALIGN_1,
        .orient = NTG_ATTACH_POLICY_SFLT_ORIENT_N,
        .thresh = NTG_ATTACH_POLICY_SFLT_THRESH_MIN,
        .enable = NTG_ATTACH_POLICY_SFLT_ENABLE_STATIC,
        .size_cap = NTG_ATTACH_POLICY_SFLT_SZCAP_NONE
    };
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_attach_policy_deinit(struct ntg_attach_policy* policy)
{
    if(!policy) return;

    if(policy->free_fn)
        policy->free_fn(policy->data);

    (*policy) = (struct ntg_attach_policy) {0};
}

void ntg_attach_policy_deinit_(void* _policy)
{
    ntg_attach_policy_deinit(_policy);
}

void ntg_attach_policy_init_flt(
        struct ntg_attach_policy* policy,
        ntg_object* anchor,
        const struct ntg_attach_policy_flt_opts* opts)
{
    if(!policy) return;
    assert(anchor);
    
    struct ntg_attach_policy_flt_dt* data = malloc(sizeof(*data));
    assert(data);

    (*data) = (struct ntg_attach_policy_flt_dt) {
        .anchor = anchor,
        .opts = (opts ? (*opts) : ntg_attach_policy_flt_opts_def())
    };

    (*policy) = (struct ntg_attach_policy) {
        .constrain_fn = flt_constrain_fn,
        .arrange_fn = flt_arrange_fn,
        .data = data,
        .free_fn = free
    };
}

void ntg_attach_policy_init_sflt(
        struct ntg_attach_policy* policy,
        ntg_object* anchor,
        const struct ntg_attach_policy_sflt_opts* opts)
{
    if(!policy) return;
    assert(anchor);
    
    struct ntg_attach_policy_sflt_dt* data = malloc(sizeof(*data));
    assert(data);

    (*data) = (struct ntg_attach_policy_sflt_dt) {
        .anchor = anchor,
        .opts = (opts ? (*opts) : ntg_attach_policy_sflt_opts_def())
    };

    (*policy) = (struct ntg_attach_policy) {
        .constrain_fn = sflt_constrain_fn,
        .arrange_fn = sflt_arrange_fn,
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
        const struct ntg_attach_constrain_ctx* ctx,
        sarena* arena)
{
    const ntg_scene* scene = ntg_object_get_scene(ctx->attacher);
    if(!scene) return 0;

    return ntg_xy_get(scene->_size, orient);
}

static struct ntg_xy root_arrange_fn(
        const void* data,
        const struct ntg_attach_arrange_ctx* ctx,
        sarena* arena)
{
    return ntg_xy(0, 0);
}

static size_t flt_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_attach_constrain_ctx* ctx,
        sarena* arena)
{
    const struct ntg_attach_policy_flt_dt* data = _data;

    const ntg_object* base = ctx->base;
    const ntg_object* attacher = ctx->attacher;
    const ntg_object* anchor = data->anchor;

    if(!base || !ntg_object_is_descendant_eq(base, data->anchor))
        return 0;

    struct ntg_object_measure attacher_measure, anchor_measure;
    attacher_measure = ntg_object_get_measure(attacher, orient);
    anchor_measure = ntg_object_get_measure(anchor, orient);

    size_t anchor_size = ntg_xy_get(anchor->_size, orient);
    size_t shrink = ntg_insets_sum(data->opts.shrink, orient);
    size_t size = _min2_size(_ssub_size(anchor_size, shrink), attacher_measure.nat_size);

    size_t thresh;
    switch(data->opts.enable)
    {
        case NTG_ATTACH_POLICY_FLT_ENABLE_MIN:
            thresh = attacher_measure.min_size;
            break;
        case NTG_ATTACH_POLICY_FLT_ENABLE_BASE_MIN:
            thresh = anchor_measure.min_size;
            break;
        case NTG_ATTACH_POLICY_FLT_ENABLE_NAT:
            thresh = attacher_measure.nat_size;
            break;
        case NTG_ATTACH_POLICY_FLT_ENABLE_BASE_NAT:
            thresh = anchor_measure.nat_size;
            break;
        case NTG_ATTACH_POLICY_FLT_ENABLE_ALWAYS:
            thresh = 0;
            break;
    }

    return (size >= thresh) ? size : 0;
}

static struct ntg_xy flt_arrange_fn(
        const void* _data,
        const struct ntg_attach_arrange_ctx* ctx,
        sarena* arena)
{
    const struct ntg_attach_policy_flt_dt* data = _data;

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

static size_t sflt_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_attach_constrain_ctx* ctx,
        sarena* arena)
{
    const struct ntg_attach_policy_sflt_dt* data = _data;
    const ntg_object* attacher = ctx->attacher;
    const ntg_object* base = ctx->attacher;
    const ntg_object* anchor = data->anchor;

    if(!ntg_object_is_descendant_eq(base, anchor))
        return 0;

    struct ntg_xy _anchor_pos;
    _anchor_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(anchor, ntg_dxy(0, 0)));
    size_t anchor_pos = ntg_xy_get(_anchor_pos, orient);

    struct ntg_object_measure attacher_measure;
    attacher_measure = ntg_object_get_measure(attacher, orient);

    const ntg_scene* scene = ntg_object_get_scene(ctx->attacher);
    size_t scene_size = (scene ? ntg_xy_get(scene->_size, orient) : 0);

    size_t anchor_size = ntg_xy_get(anchor->_size, orient);
    if(anchor_size == 0) return 0;

    size_t available_space;
    bool capped = (data->opts.size_cap == NTG_ATTACH_POLICY_SFLT_SZCAP_ANCHOR);
    switch(data->opts.orient)
    {
        case NTG_ATTACH_POLICY_SFLT_ORIENT_N:
            available_space = (orient == NTG_ORIENT_H) ?
                (capped ? anchor_size : scene_size) :
                anchor_pos;
            break;

        case NTG_ATTACH_POLICY_SFLT_ORIENT_E:
            available_space = (orient == NTG_ORIENT_V) ?
                (capped ? anchor_size : scene_size) :
                _ssub_size(scene_size, anchor_size + anchor_pos);

            break;

        case NTG_ATTACH_POLICY_SFLT_ORIENT_S:
            available_space = (orient == NTG_ORIENT_H) ?
                (capped ? anchor_size : scene_size) :
                _ssub_size(scene_size, anchor_size + anchor_pos);
            break;

        case NTG_ATTACH_POLICY_SFLT_ORIENT_W:
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
        case NTG_ATTACH_POLICY_SFLT_THRESH_MIN:
            thresh_size = attacher_measure.min_size;
            break;
        case NTG_ATTACH_POLICY_SFLT_THRESH_NAT:
            thresh_size = attacher_measure.nat_size;
            break;
        case NTG_ATTACH_POLICY_SFLT_THRESH_ALWAYS:
            thresh_size = 0;
            break;
        default:
            assert(0);
    }

    if(available_space < thresh_size)
        return 0;
    else
        return _min2_size(available_space, attacher_measure.nat_size);
}

static struct ntg_xy sflt_arrange_fn(
        const void* _data,
        const struct ntg_attach_arrange_ctx* ctx,
        sarena* arena)
{
    const struct ntg_attach_policy_sflt_dt* data = _data;
    const ntg_object* attacher = ctx->attacher;
    const ntg_object* base = ctx->attacher;
    const ntg_object* anchor = data->anchor;

    if(!ntg_object_is_descendant_eq(base, anchor))
        return ntg_xy(0, 0);

    struct ntg_xy anchor_pos;
    anchor_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(anchor, ntg_dxy(0, 0)));

    const ntg_scene* scene = ntg_object_get_scene(ctx->attacher);
    struct ntg_xy scene_size = (scene ? scene->_size : ntg_xy(0, 0));

    size_t align_total_size, align_content_size;

    ntg_orient align_orient = (
            ((data->opts.orient == NTG_ATTACH_POLICY_SFLT_ORIENT_N) ||
                (data->opts.orient == NTG_ATTACH_POLICY_SFLT_ORIENT_S)) ?
            NTG_ORIENT_H : NTG_ORIENT_V);

    bool capped = (data->opts.size_cap == NTG_ATTACH_POLICY_SFLT_SZCAP_ANCHOR);

    align_total_size = (
            capped ?
            ntg_xy_get(anchor->_size, align_orient) :
            ntg_xy_get(scene_size, align_orient));

    align_content_size = ntg_xy_get(attacher->_size, align_orient);

    size_t align_pos = ntg_align_offset(align_content_size,
            align_total_size, data->opts.align);

    struct ntg_xy pos = ntg_xy(0, 0);
    switch(data->opts.orient)
    {
        case NTG_ATTACH_POLICY_SFLT_ORIENT_N:
            pos.x = (capped ? anchor_pos.x + align_pos : align_pos);
            pos.y = _ssub_size(anchor_pos.y, attacher->_size.y);
            break;
        case NTG_ATTACH_POLICY_SFLT_ORIENT_E:
            pos.y = (capped ? anchor_pos.y + align_pos : align_pos);
            pos.x = anchor_pos.x + anchor->_size.x;
            break;
        case NTG_ATTACH_POLICY_SFLT_ORIENT_S:
            pos.x = (capped ? anchor_pos.x + align_pos : align_pos);
            pos.y = anchor_pos.y + anchor->_size.y;
            break;
        case NTG_ATTACH_POLICY_SFLT_ORIENT_W:
            pos.y = (capped ? anchor_pos.y + align_pos : align_pos);
            pos.x = _ssub_size(anchor_pos.x, attacher->_size.x);
            break;
    }

    return ntg_xy_clamp(ntg_xy(0, 0), pos, scene_size);
}
