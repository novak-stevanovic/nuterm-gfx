#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static size_t float_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy float_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

static void float_deinit_fn(ntg_anchor_policy* ap);

static const struct ntg_anchor_policy_vtable VTABLE = {
    .constrain_fn = float_constrain_fn,
    .arrange_fn = float_arrange_fn,
    .deinit_fn = float_deinit_fn
};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_float_opts ntg_float_opts_default(void)
{
    return (struct ntg_float_opts) {
        .enable = NTG_FLOAT_ENABLE_MIN,
        .shrink = ntg_insets(0, 0, 0, 0),
        .prim_align = NTG_ALIGN_1,
        .sec_align = NTG_ALIGN_1
    };
}

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

int ntg_float_init(
        ntg_float* float_ap,
        const struct ntg_float_opts* opts)
{
    if(!float_ap)
        return NTG_ERR_INV_ARG;

    int _status = ntg_anchor_policy_init_inherit(&float_ap->__base, &VTABLE);
    if(_status)
        return _status;

    float_ap->_opts = opts ? (*opts) : ntg_float_opts_default();
    return 0;
}

void ntg_float_deinit(ntg_float* float_ap)
{
    if(!float_ap) return;

    float_ap->_opts = ntg_float_opts_default();
    ntg_anchor_policy_deinit(&float_ap->__base);
}

void ntg_float_deinit_void(void* _float_ap)
{
    ntg_float_deinit(_float_ap);
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static size_t float_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx || !ctx->root || !ctx->base) return 0;

    const ntg_float* float_ap = (const ntg_float*)ap;
    const struct ntg_float_opts* opts = &float_ap->_opts;
    const ntg_object* base = ctx->base;
    const ntg_object* root = ctx->root;

    struct ntg_object_measure root_measure;
    struct ntg_object_measure base_measure;
    root_measure = ntg_object_get_measure(root, orient);
    base_measure = ntg_object_get_measure(base, orient);

    size_t base_size = ntg_xy_get(base->_size, orient);
    size_t shrink = ntg_insets_sum(opts->shrink, orient);
    size_t size = _min2_size(
            _sub2_size(base_size, shrink),
            root_measure.nat_size);

    size_t thresh;
    switch(opts->enable)
    {
        case NTG_FLOAT_ENABLE_MIN:
            thresh = root_measure.min_size;
            break;
        case NTG_FLOAT_ENABLE_BASE_MIN:
            thresh = base_measure.min_size;
            break;
        case NTG_FLOAT_ENABLE_NAT:
            thresh = root_measure.nat_size;
            break;
        case NTG_FLOAT_ENABLE_BASE_NAT:
            thresh = base_measure.nat_size;
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

    if(!ctx || !ctx->base) return ntg_xy(0, 0);
    if(ntg_xy_size_is_zero(ctx->size)) return ntg_xy(0, 0);

    const ntg_float* float_ap = (const ntg_float*)ap;
    const struct ntg_float_opts* opts = &float_ap->_opts;
    const ntg_object* base = ctx->base;

    struct ntg_xy shrink = ntg_xy(
            ntg_insets_hsum(opts->shrink),
            ntg_insets_vsum(opts->shrink));
    struct ntg_xy base_size = ntg_xy_sub(base->_size, shrink);

    struct ntg_xy align_offset = ntg_xy(
            ntg_align_offset(ctx->size.x, base_size.x, opts->prim_align),
            ntg_align_offset(ctx->size.y, base_size.y, opts->sec_align));

    align_offset.x += opts->shrink.w;
    align_offset.y += opts->shrink.n;

    struct ntg_xy pos = ntg_xy_from_dxy(
            ntg_object_map_to_scene(base, ntg_dxy(0, 0)));

    return ntg_xy_add(pos, align_offset);
}

static void float_deinit_fn(ntg_anchor_policy* ap)
{
    ntg_float_deinit((ntg_float*)ap);
}
