#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static size_t sidefloat_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy sidefloat_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

static void sidefloat_deinit_fn(ntg_entity* _ap);

static const struct ntg_anchor_policy_vtable VTABLE = {
    .constrain_fn = sidefloat_constrain_fn,
    .arrange_fn = sidefloat_arrange_fn,
    .base.deinit_fn = sidefloat_deinit_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_sidefloat_opts ntg_sidefloat_opts_default(void)
{
    return (struct ntg_sidefloat_opts) {
        .align = NTG_ALIGN_1,
        .side = NTG_SIDE_N,
        .thresh = NTG_SIDEFLOAT_THRESH_MIN,
        .size_cap = NTG_SIDEFLOAT_SIZE_CAP_NONE
    };
}

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_sidefloat_init(
        ntg_sidefloat* sidefloat_ap,
        const struct ntg_sidefloat_opts* opts)
{
    if(!sidefloat_ap)
        return NTG_ERR_INV_ARG;

    int _status = ntg_anchor_policy_init_inherit(&sidefloat_ap->_base, &VTABLE, &NTG_TYPE_SIDEFLOAT);
    if(_status)
        return _status;

    sidefloat_ap->ro.opts = opts ? (*opts) : ntg_sidefloat_opts_default();
    return 0;
}

int ntg_sidefloat_deinit(ntg_sidefloat* sidefloat_ap)
{
    if(!sidefloat_ap) return NTG_ERR_INV_ARG;

    sidefloat_ap->ro.opts = ntg_sidefloat_opts_default();
    ntg_anchor_policy_deinit(&sidefloat_ap->_base);

    return 0;
}

void ntg_sidefloat_deinit_void(void* _sidefloat_ap)
{
    ntg_sidefloat_deinit(_sidefloat_ap);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static size_t sidefloat_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx || !ctx->root) return 0;

    const ntg_sidefloat* sidefloat_ap = (const ntg_sidefloat*)ap;
    const struct ntg_sidefloat_opts* opts = &sidefloat_ap->ro.opts;
    const ntg_object* root = ctx->root;

    size_t base_pos = ctx->base_pos;

    struct ntg_object_measure root_measure;
    root_measure = ntg_object_get_measure(root, orient);

    const ntg_scene* scene = ntg_object_get_scene(root);
    size_t scene_size = scene ? ntg_xy_get(scene->ro.size, orient) : 0;

    size_t base_size = ctx->base_size;
    if(base_size == 0) return 0;

    bool capped = opts->size_cap == NTG_SIDEFLOAT_SIZE_CAP_ANCHOR;
    size_t available_space;

    switch(opts->side)
    {
        case NTG_SIDE_N:
            available_space = (orient == NTG_ORIENT_H) ?
                    (capped ? base_size : scene_size) : base_pos;
            break;
        case NTG_SIDE_E:
            available_space = (orient == NTG_ORIENT_V) ?
                    (capped ? base_size : scene_size) :
                    _sub2_size(scene_size, base_size + base_pos);
            break;
        case NTG_SIDE_S:
            available_space = (orient == NTG_ORIENT_H) ?
                    (capped ? base_size : scene_size) :
                    _sub2_size(scene_size, base_size + base_pos);
            break;
        case NTG_SIDE_W:
            available_space = (orient == NTG_ORIENT_V) ?
                    (capped ? base_size : scene_size) : base_pos;
            break;
        default:
            return 0;
    }

    size_t thresh_size;
    switch(opts->thresh)
    {
        case NTG_SIDEFLOAT_THRESH_MIN:
            thresh_size = root_measure.min_size;
            break;
        case NTG_SIDEFLOAT_THRESH_NAT:
            thresh_size = root_measure.nat_size;
            break;
        case NTG_SIDEFLOAT_THRESH_ALWAYS:
            thresh_size = 0;
            break;
        default:
            return 0;
    }

    if(available_space < thresh_size)
        return 0;

    return _min2_size(available_space, root_measure.nat_size);
}

static struct ntg_xy sidefloat_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx) return ntg_xy(0, 0);

    const ntg_sidefloat* sidefloat_ap = (const ntg_sidefloat*)ap;
    const struct ntg_sidefloat_opts* opts = &sidefloat_ap->ro.opts;

    struct ntg_xy pos = ntg_xy(0, 0);
    struct ntg_dxy size_diff = ntg_dxy(
            (ssize_t)ctx->size.x - (ssize_t)ctx->base_size.x,
            (ssize_t)ctx->size.y - (ssize_t)ctx->base_size.y);

    switch(opts->side)
    {
        case NTG_SIDE_N:
            pos.x = (size_diff.x > 0) ?
                    _sub2_size(ctx->base_pos.x, size_diff.x) :
                    ctx->base_pos.x + ntg_align_offset_d(
                            ctx->size.x,
                            ctx->base_size.x,
                            opts->align);
            pos.y = _sub2_size(ctx->base_pos.y, ctx->size.y);
            break;
        case NTG_SIDE_E:
            pos.x = ctx->base_pos.x + ctx->base_size.x;
            pos.y = (size_diff.y > 0) ?
                    _sub2_size(ctx->base_pos.y, size_diff.y) :
                    ctx->base_pos.y + ntg_align_offset_d(
                            ctx->size.y,
                            ctx->base_size.y,
                            opts->align);
            break;
        case NTG_SIDE_S:
            pos.x = (size_diff.x > 0) ?
                    _sub2_size(ctx->base_pos.x, size_diff.x) :
                    ctx->base_pos.x + ntg_align_offset_d(
                            ctx->size.x,
                            ctx->base_size.x,
                            opts->align);
            pos.y = ctx->base_pos.y + ctx->base_size.y;
            break;
        case NTG_SIDE_W:
            pos.x = _sub2_size(ctx->base_pos.x, ctx->size.x);
            pos.y = (size_diff.y > 0) ?
                    _sub2_size(ctx->base_pos.y, size_diff.y) :
                    ctx->base_pos.y + ntg_align_offset_d(
                            ctx->size.y,
                            ctx->base_size.y,
                            opts->align);
            break;
        default:
            break;
    }

    return pos;
}

static void sidefloat_deinit_fn(ntg_entity* _ap)
{
    ntg_sidefloat_deinit((ntg_sidefloat*)_ap);
}
