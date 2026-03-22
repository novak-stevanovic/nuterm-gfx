#include "ntg.h"
#include "shared/ntg_shared_internal.h"
#include <stdlib.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

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
        .side = NTG_SIDE_N,
        .thresh = NTG_SIDEFLOAT_POLICY_THRESH_MIN,
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
        const struct ntg_float_policy_opts* opts)
{
    if(!policy) return;
    
    struct ntg_float_policy_opts* opts_copy = malloc(sizeof(*opts_copy));
    assert(opts_copy);

    (*opts_copy) = (opts ? (*opts) : ntg_float_policy_opts_def());

    (*policy) = (struct ntg_anchor_policy) {
        .constrain_fn = float_constrain_fn,
        .arrange_fn = float_arrange_fn,
        .data = opts_copy,
        .free_fn = free
    };
}

void ntg_anchor_policy_init_sidefloat(
        struct ntg_anchor_policy* policy,
        const struct ntg_sidefloat_policy_opts* opts)
{
    if(!policy) return;
    
    struct ntg_sidefloat_policy_opts* opts_copy = malloc(sizeof(*opts_copy));
    assert(opts_copy);

    (*opts_copy) = (opts ? (*opts) : ntg_sidefloat_policy_opts_def());

    (*policy) = (struct ntg_anchor_policy) {
        .constrain_fn = sidefloat_constrain_fn,
        .arrange_fn = sidefloat_arrange_fn,
        .data = opts_copy,
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
    const struct ntg_float_policy_opts* opts = _data;

    const ntg_object* base = ctx->base;
    const ntg_object* root = ctx->root;

    if(!base) return 0;

    struct ntg_object_measure root_measure, base_measure;
    root_measure = ntg_object_get_measure(root, orient);
    base_measure = ntg_object_get_measure(base, orient);

    size_t base_size = ntg_xy_get(base->_size, orient);
    size_t shrink = ntg_insets_sum(opts->shrink, orient);
    size_t size = _min2_size(_ssub_size(base_size, shrink), root_measure.nat_size);

    size_t thresh;
    switch(opts->enable)
    {
        case NTG_FLOAT_POLICY_ENABLE_MIN:
            thresh = root_measure.min_size;
            break;
        case NTG_FLOAT_POLICY_ENABLE_BASE_MIN:
            thresh = base_measure.min_size;
            break;
        case NTG_FLOAT_POLICY_ENABLE_NAT:
            thresh = root_measure.nat_size;
            break;
        case NTG_FLOAT_POLICY_ENABLE_BASE_NAT:
            thresh = base_measure.nat_size;
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
    const struct ntg_float_policy_opts* opts = _data;

    const ntg_object* base = ctx->base;

    if(!base) return ntg_xy(0, 0);

    if(ntg_xy_size_is_zero(ctx->size))
        return ntg_xy(0, 0);

    struct ntg_xy shrink = ntg_xy(
        ntg_insets_hsum(opts->shrink),
        ntg_insets_vsum(opts->shrink));
    struct ntg_xy base_size = ntg_xy_sub(base->_size, shrink);

    struct ntg_xy align_offset = ntg_xy(
            ntg_align_offset(ctx->size.x, base_size.x, opts->prim_align),
            ntg_align_offset(ctx->size.y, base_size.y, opts->sec_align));

    align_offset.x += opts->shrink.w;
    align_offset.y += opts->shrink.n;

    struct ntg_xy
    pos = ntg_xy_from_dxy(ntg_object_map_to_scene(base, ntg_dxy(0, 0)));

    return ntg_xy_add(pos, align_offset);
}

static size_t sidefloat_constrain_fn(
        const void* _data,
        ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    const struct ntg_sidefloat_policy_opts* opts = _data;

    const ntg_object* root = ctx->root;
    const ntg_object* base = ctx->base;

    if(!base) return 0;

    struct ntg_xy _base_pos;
    _base_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(base, ntg_dxy(0, 0)));
    size_t base_pos = ntg_xy_get(_base_pos, orient);

    struct ntg_object_measure root_measure;
    root_measure = ntg_object_get_measure(root, orient);

    const ntg_scene* scene = ntg_object_get_scene(ctx->root);
    size_t scene_size = (scene ? ntg_xy_get(scene->_size, orient) : 0);

    size_t base_size = ntg_xy_get(base->_size, orient);
    if(base_size == 0) return 0;

    size_t available_space;
    bool capped = (opts->size_cap == NTG_SIDEFLOAT_POLICY_SZCAP_ANCHOR);
    switch(opts->side)
    {
        case NTG_SIDE_N:
            available_space = (orient == NTG_ORIENT_H) ?
                (capped ? base_size : scene_size) :
                base_pos;
            break;

        case NTG_SIDE_E:
            available_space = (orient == NTG_ORIENT_V) ?
                (capped ? base_size : scene_size) :
                _ssub_size(scene_size, base_size + base_pos);

            break;

        case NTG_SIDE_S:
            available_space = (orient == NTG_ORIENT_H) ?
                (capped ? base_size : scene_size) :
                _ssub_size(scene_size, base_size + base_pos);
            break;

        case NTG_SIDE_W:
            available_space = (orient == NTG_ORIENT_V) ?
                (capped ? base_size : scene_size) :
                base_pos;
            break;

        default:
            assert(0);
    }

    size_t thresh_size;
    switch(opts->thresh)
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
    const struct ntg_sidefloat_policy_opts* opts = _data;

    const ntg_object* root = ctx->root;
    const ntg_object* base = ctx->base;

    if(!base) return ntg_xy(0, 0);

    // struct ntg_xy base_pos;
    // base_pos = ntg_xy_from_dxy(ntg_object_map_to_scene(base, ntg_dxy(0, 0)));

    // const ntg_scene* scene = ntg_object_get_scene(ctx->root);
    // struct ntg_xy scene_size = (scene ? scene->_size : ntg_xy(0, 0));

    struct ntg_xy pos = ntg_xy(0, 0);

    struct ntg_dxy size_diff = ntg_dxy(
            (ssize_t)root->_size.x - (ssize_t)base->_size.x,
            (ssize_t)root->_size.y - (ssize_t)base->_size.y);

    switch(opts->side)
    {
        case NTG_SIDE_N:
            pos.x = (size_diff.x > 0) ?
                _ssub_size(base->_pos.x, size_diff.x) :
                base->_pos.x + ntg_align_offset_d(root->_size.x, base->_size.x, opts->align);
            pos.y = _ssub_size(base->_pos.y, root->_size.y);
            break;
        case NTG_SIDE_E:
            pos.x = base->_pos.x + base->_size.x;
            pos.y = (size_diff.y > 0) ? 
                _ssub_size(base->_pos.y, size_diff.y) :
                base->_pos.y + ntg_align_offset_d(root->_size.y, base->_size.y, opts->align);
            break;
        case NTG_SIDE_S:
            pos.x = (size_diff.x > 0) ?
                _ssub_size(base->_pos.x, size_diff.x) :
                base->_pos.x + ntg_align_offset_d(root->_size.x, base->_size.x, opts->align);
            pos.y = base->_pos.y + base->_size.y;
            break;
        case NTG_SIDE_W:
            pos.x = _ssub_size(base->_pos.x, root->_size.x);
            pos.y = (size_diff.y > 0) ? 
                _ssub_size(base->_pos.y, size_diff.y) :
                base->_pos.y + ntg_align_offset_d(root->_size.y, base->_size.y, opts->align);
            break;
    }

    return pos;
}
