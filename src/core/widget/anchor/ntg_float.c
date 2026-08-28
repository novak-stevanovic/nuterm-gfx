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

static struct ntg_xy float_arrange_fn(
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

int ntg_float_init(ntg_float* float_ap, const struct ntg_float_opts* opts)
{
    if(!float_ap)
        return NTG_ERR_INV_ARG;

    int status = ntg_anchor_policy_init_inherit(
            ntg_ap(float_ap),
            &VTABLE,
            &NTG_TYPE_FLOAT);
    NTG_POST_INHERIT_CHECK(status);

    ntg_object_zero(float_ap);

    return ntg_float_set_opts(float_ap, opts);
}

int ntg_float_deinit(ntg_float* float_ap)
{
    if(!float_ap) return NTG_ERR_INV_ARG;

    ntg_object_zero(float_ap);
    ntg_anchor_policy_deinit(&float_ap->_base);

    return 0;
}


int ntg_float_set_opts(ntg_float* float_ap, const struct ntg_float_opts* opts)
{
    if(!float_ap) return NTG_ERR_INV_ARG;

    struct ntg_float_opts opts_final =
            (opts ? (*opts) : NTG_FLOAT_OPTS_ZERO);

    if((float_ap->ro.opts.enable == opts_final.enable) &&
       ntg_insets_are_eql(float_ap->ro.opts.shrink, opts_final.shrink) &&
       (float_ap->ro.opts.prim_align == opts_final.prim_align) &&
       (float_ap->ro.opts.sec_align == opts_final.sec_align))
    {
        return 0;
    }

    float_ap->ro.opts.enable = opts_final.enable;
    float_ap->ro.opts.shrink = opts_final.shrink;
    float_ap->ro.opts.prim_align = opts_final.prim_align;
    float_ap->ro.opts.sec_align = opts_final.sec_align;

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

    const ntg_float* float_ap = (const ntg_float*)ap;
    const ntg_widget* root = ctx->root;

    struct ntg_widget_measure root_measure;
    root_measure = ntg_widget_get_measure(root, orient);

    size_t base_size = ctx->base_size;
    size_t shrink = ntg_insets_sum(float_ap->ro.opts.shrink, orient);
    size_t size = _min2_size(
            _sub2_size(base_size, shrink),
            root_measure.nat_size);

    size_t thresh;
    switch(float_ap->ro.opts.enable)
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

static struct ntg_xy float_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx) return ntg_xy(0, 0);
    if(ntg_xy_is_zero_any(ctx->size)) return ntg_xy(0, 0);

    const ntg_float* float_ap = (const ntg_float*)ap;

    struct ntg_xy shrink = ntg_xy(
            ntg_insets_hsum(float_ap->ro.opts.shrink),
            ntg_insets_vsum(float_ap->ro.opts.shrink));
    struct ntg_xy base_size = ntg_xy_sub(ctx->base_size, shrink);

    struct ntg_xy align_offset = ntg_xy(
            ntg_align_offset(ctx->size.x, base_size.x, float_ap->ro.opts.prim_align),
            ntg_align_offset(ctx->size.y, base_size.y, float_ap->ro.opts.sec_align));

    align_offset.x += float_ap->ro.opts.shrink.w;
    align_offset.y += float_ap->ro.opts.shrink.n;

    return ntg_xy_add(ctx->base_pos, align_offset);
}

static void float_deinit_fn(ntg_object* _ap)
{
    ntg_float_deinit((ntg_float*)_ap);
}
