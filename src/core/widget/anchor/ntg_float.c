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

static size_t float_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static ntg_xy float_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

static void float_deinit_fn(ntg_object* _ap);

static const struct ntg_anchor_policy_vtable VTABLE = {
    .constrain_fn = float_constrain_fn,
    .arrange_fn = float_arrange_fn,
    .base.deinit_fn = float_deinit_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_float_init(ntg_float* flt, const struct ntg_float_opts* opts)
{
    if(!flt)
        return NTG_ERR_INV_ARG;

    int status = ntg_anchor_policy_init_inherit(
            ntg_ap(flt),
            &VTABLE,
            &NTG_TYPE_FLOAT);
    NTG_POST_INHERIT_CHECK(status);

    ntg_object_zero(flt);

    return ntg_float_set_opts(flt, opts);
}

int ntg_float_deinit(ntg_float* flt)
{
    if(!flt) return NTG_ERR_INV_ARG;

    ntg_object_zero(flt);
    ntg_anchor_policy_deinit(&flt->_base);

    return 0;
}


int ntg_float_set_opts(ntg_float* flt, const struct ntg_float_opts* opts)
{
    if(!flt) return NTG_ERR_INV_ARG;

    struct ntg_float_opts opts_final =
            (opts ? (*opts) : NTG_FLOAT_OPTS_ZERO);

    if((flt->ro.opts.enable == opts_final.enable) &&
       ntg_insets_are_eql(flt->ro.opts.shrink, opts_final.shrink) &&
       (flt->ro.opts.prim_align == opts_final.prim_align) &&
       (flt->ro.opts.sec_align == opts_final.sec_align))
    {
        return 0;
    }

    flt->ro.opts.enable = opts_final.enable;
    flt->ro.opts.shrink = opts_final.shrink;
    flt->ro.opts.prim_align = opts_final.prim_align;
    flt->ro.opts.sec_align = opts_final.sec_align;

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

static size_t float_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx || !ctx->root) return 0;

    const ntg_float* flt = (const ntg_float*)ap;
    const ntg_widget* root = ctx->root;

    struct ntg_widget_measure root_measure;
    root_measure = ntg_widget_get_measure(root, orient);

    size_t base_size = ctx->base_size;
    size_t shrink = ntg_insets_sum(flt->ro.opts.shrink, orient);
    size_t size = ntg_min2_size(
            ntg_sub2_size(base_size, shrink),
            root_measure.nat_size);

    size_t thresh;
    switch(flt->ro.opts.enable)
    {
        case NTG_FLOAT_ENABLE_MIN:
            thresh = root_measure.min_size;
            break;
        case NTG_FLOAT_ENABLE_BASE_MIN:
            thresh = ctx->base_min_size;
            break;
        case NTG_FLOAT_ENABLE_NAT:
            thresh = root_measure.nat_size;
            break;
        case NTG_FLOAT_ENABLE_BASE_NAT:
            thresh = ctx->base_nat_size;
            break;
        case NTG_FLOAT_ENABLE_ALWAYS:
            thresh = 0;
            break;
        default:
            return 0;
    }

    return (size >= thresh) ? size : 0;
}

static ntg_xy float_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx) return ntg_xy_new(0, 0);
    if(ntg_xy_is_zero_any(ctx->size)) return ntg_xy_new(0, 0);

    const ntg_float* flt = (const ntg_float*)ap;

    ntg_xy shrink = ntg_xy_new(
            ntg_insets_hsum(flt->ro.opts.shrink),
            ntg_insets_vsum(flt->ro.opts.shrink));
    ntg_xy base_size = ntg_xy_sub(ctx->base_size, shrink);

    ntg_xy align_offset = ntg_xy_new(
            ntg_align_offset_size(ctx->size.ro.x, base_size.ro.x, flt->ro.opts.prim_align),
            ntg_align_offset_size(ctx->size.ro.y, base_size.ro.y, flt->ro.opts.sec_align));

    align_offset = ntg_xy_add(
            align_offset,
            ntg_xy_new(flt->ro.opts.shrink.ro.w, flt->ro.opts.shrink.ro.n));

    return ntg_xy_add(ctx->base_pos, align_offset);
}

static void float_deinit_fn(ntg_object* _ap)
{
    ntg_float_deinit((ntg_float*)_ap);
}
