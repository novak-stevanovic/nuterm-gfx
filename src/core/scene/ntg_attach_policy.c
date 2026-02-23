#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

struct ntg_attach_policy_flt_dt
{
    ntg_object* base;
    struct ntg_attach_policy_flt_opts opts;
};

struct ntg_attach_policy_sflt_dt
{
    ntg_object* base;
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
        .enable = NTG_ATTACH_POLICY_SFLT_ENABLE_STATIC
    };
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* BASE */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/* PRESETS */
/* -------------------------------------------------------------------------- */

void ntg_attach_policy_init_flt(
        struct ntg_attach_policy* policy,
        ntg_object* base,
        const struct ntg_attach_policy_flt_opts* opts)
{
    if(!policy) return;
    assert(base);
    
    struct ntg_attach_policy_flt_dt* data = malloc(sizeof(*data));
    assert(data);

    (*data) = (struct ntg_attach_policy_flt_dt) {
        .base = base,
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
        ntg_object* base,
        const struct ntg_attach_policy_sflt_opts* opts)
{
    if(!policy) return;
    assert(base);
    
    struct ntg_attach_policy_sflt_dt* data = malloc(sizeof(*data));
    assert(data);

    (*data) = (struct ntg_attach_policy_sflt_dt) {
        .base = base,
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
    return ctx->scene_size;
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

    const ntg_object* base_layer = ctx->base_layer;
    const ntg_object* attacher_layer = ctx->attacher_layer;

    if(!base_layer || !ntg_object_is_descendant_eq(base_layer, data->base))
        return 0;

    size_t attacher_min_size = ntg_xy_get(attacher_layer->_min_size, orient);
    size_t attacher_nat_size = ntg_xy_get(attacher_layer->_nat_size, orient);
    size_t base_min_size = ntg_xy_get(data->base->_min_size, orient);
    size_t base_nat_size = ntg_xy_get(data->base->_nat_size, orient);

    size_t base_size = ntg_xy_get(data->base->_size, orient);
    size_t shrink = ntg_insets_sum(data->opts.shrink, orient);
    size_t size = _min2_size(_ssub_size(base_size, shrink), attacher_nat_size);

    size_t thresh;
    switch(data->opts.enable)
    {
        case NTG_ATTACH_POLICY_FLT_ENABLE_MIN:
            thresh = attacher_min_size;
            break;
        case NTG_ATTACH_POLICY_FLT_ENABLE_BASE_MIN:
            thresh = base_min_size;
            break;
        case NTG_ATTACH_POLICY_FLT_ENABLE_NAT:
            thresh = attacher_nat_size;
            break;
        case NTG_ATTACH_POLICY_FLT_ENABLE_BASE_NAT:
            thresh = base_nat_size;
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

    const ntg_object* base_layer = ctx->base_layer;

    if(!base_layer || !ntg_object_is_descendant_eq(base_layer, data->base))
        return ntg_xy(0, 0);

    if(ntg_xy_size_is_zero(ctx->size))
        return ntg_xy(0, 0);

    struct ntg_xy shrink = ntg_xy(
        ntg_insets_hsum(data->opts.shrink),
        ntg_insets_vsum(data->opts.shrink));
    struct ntg_xy base_size = ntg_xy_sub(data->base->_size, shrink);

    struct ntg_xy align_offset = ntg_xy(
            ntg_align_offset(ctx->size.x, base_size.x, data->opts.prim_align),
            ntg_align_offset(ctx->size.y, base_size.y, data->opts.sec_align));

    align_offset.x += data->opts.shrink.w;
    align_offset.y += data->opts.shrink.n;

    struct ntg_xy
    pos = ntg_xy_from_dxy(ntg_object_map_to_scene(data->base, ntg_dxy(0, 0)));

    return ntg_xy_add(pos, align_offset);
}

static size_t sflt_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_attach_constrain_ctx* ctx,
        sarena* arena)
{
    assert(0);
}

static struct ntg_xy sflt_arrange_fn(
        const void* _data,
        const struct ntg_attach_arrange_ctx* ctx,
        sarena* arena)
{
    assert(0);
}
