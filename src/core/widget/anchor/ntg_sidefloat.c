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

static void sidefloat_deinit_fn(ntg_object* _ap);

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
/* FUNCTIONS */
/* ========================================================================== */

int ntg_sidefloat_init(ntg_sidefloat* sidefloat_ap, const struct ntg_sidefloat_opts* opts)
{
    if(!sidefloat_ap)
        return NTG_ERR_INV_ARG;

    int status = ntg_anchor_policy_init_inherit(
            ntg_ap(sidefloat_ap),
            &VTABLE,
            &NTG_TYPE_SIDEFLOAT);
    NTG_POST_INHERIT_CHECK(status);

    ntg_object_zero(sidefloat_ap);

    return ntg_sidefloat_set_opts(sidefloat_ap, opts);
}

int ntg_sidefloat_deinit(ntg_sidefloat* sidefloat_ap)
{
    if(!sidefloat_ap) return NTG_ERR_INV_ARG;

    ntg_object_zero(sidefloat_ap);
    ntg_anchor_policy_deinit(&sidefloat_ap->_base);

    return 0;
}


int ntg_sidefloat_set_opts(
        ntg_sidefloat* sidefloat_ap,
        const struct ntg_sidefloat_opts* opts)
{
    if(!sidefloat_ap) return NTG_ERR_INV_ARG;

    struct ntg_sidefloat_opts opts_final =
            (opts ? (*opts) : NTG_SIDEFLOAT_OPTS_ZERO);

    if((sidefloat_ap->ro.opts.align == opts_final.align) &&
       (sidefloat_ap->ro.opts.side == opts_final.side) &&
       (sidefloat_ap->ro.opts.thresh == opts_final.thresh) &&
       (sidefloat_ap->ro.opts.size_cap == opts_final.size_cap))
    {
        return 0;
    }

    sidefloat_ap->ro.opts.align = opts_final.align;
    sidefloat_ap->ro.opts.side = opts_final.side;
    sidefloat_ap->ro.opts.thresh = opts_final.thresh;
    sidefloat_ap->ro.opts.size_cap = opts_final.size_cap;

    return 0;
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
    const ntg_widget* root = ctx->root;

    size_t base_pos = ctx->base_pos;

    struct ntg_widget_measure root_measure;
    root_measure = ntg_widget_get_measure(root, orient);

    const ntg_scene* scene = ntg_widget_get_scene(root);
    size_t scene_size = scene ? ntg_xy_get(scene->ro.size, orient) : 0;

    size_t base_size = ctx->base_size;
    if(base_size == 0) return 0;

    bool capped = sidefloat_ap->ro.opts.size_cap == NTG_SIDEFLOAT_SIZE_CAP_ANCHOR;
    size_t available_space;

    switch(sidefloat_ap->ro.opts.side)
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
    switch(sidefloat_ap->ro.opts.thresh)
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

    struct ntg_xy pos = ntg_xy(0, 0);
    struct ntg_dxy size_diff = ntg_dxy(
            (ssize_t)ctx->size.x - (ssize_t)ctx->base_size.x,
            (ssize_t)ctx->size.y - (ssize_t)ctx->base_size.y);

    switch(sidefloat_ap->ro.opts.side)
    {
        case NTG_SIDE_N:
            pos.x = (size_diff.x > 0) ?
                    _sub2_size(ctx->base_pos.x, size_diff.x) :
                    ctx->base_pos.x + ntg_align_offset_d(
                            ctx->size.x,
                            ctx->base_size.x,
                            sidefloat_ap->ro.opts.align);
            pos.y = _sub2_size(ctx->base_pos.y, ctx->size.y);
            break;
        case NTG_SIDE_E:
            pos.x = ctx->base_pos.x + ctx->base_size.x;
            pos.y = (size_diff.y > 0) ?
                    _sub2_size(ctx->base_pos.y, size_diff.y) :
                    ctx->base_pos.y + ntg_align_offset_d(
                            ctx->size.y,
                            ctx->base_size.y,
                            sidefloat_ap->ro.opts.align);
            break;
        case NTG_SIDE_S:
            pos.x = (size_diff.x > 0) ?
                    _sub2_size(ctx->base_pos.x, size_diff.x) :
                    ctx->base_pos.x + ntg_align_offset_d(
                            ctx->size.x,
                            ctx->base_size.x,
                            sidefloat_ap->ro.opts.align);
            pos.y = ctx->base_pos.y + ctx->base_size.y;
            break;
        case NTG_SIDE_W:
            pos.x = _sub2_size(ctx->base_pos.x, ctx->size.x);
            pos.y = (size_diff.y > 0) ?
                    _sub2_size(ctx->base_pos.y, size_diff.y) :
                    ctx->base_pos.y + ntg_align_offset_d(
                            ctx->size.y,
                            ctx->base_size.y,
                            sidefloat_ap->ro.opts.align);
            break;
        default:
            break;
    }

    return pos;
}

static void sidefloat_deinit_fn(ntg_object* _ap)
{
    ntg_sidefloat_deinit((ntg_sidefloat*)_ap);
}
