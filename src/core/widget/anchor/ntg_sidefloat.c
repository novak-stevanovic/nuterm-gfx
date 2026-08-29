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

static ntg_xy sidefloat_arrange_fn(
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

int ntg_sidefloat_init(ntg_sidefloat* sflt, const struct ntg_sidefloat_opts* opts)
{
    if(!sflt)
        return NTG_ERR_INV_ARG;

    int status = ntg_anchor_policy_init_inherit(
            ntg_ap(sflt),
            &VTABLE,
            &NTG_TYPE_SIDEFLOAT);
    NTG_POST_INHERIT_CHECK(status);

    ntg_object_zero(sflt);

    return ntg_sidefloat_set_opts(sflt, opts);
}

int ntg_sidefloat_deinit(ntg_sidefloat* sflt)
{
    if(!sflt) return NTG_ERR_INV_ARG;

    ntg_object_zero(sflt);
    ntg_anchor_policy_deinit(&sflt->_base);

    return 0;
}


int ntg_sidefloat_set_opts(
        ntg_sidefloat* sflt,
        const struct ntg_sidefloat_opts* opts)
{
    if(!sflt) return NTG_ERR_INV_ARG;

    struct ntg_sidefloat_opts opts_final = (opts ? (*opts) : NTG_SIDEFLOAT_OPTS_ZERO);

    if((sflt->ro.opts.align == opts_final.align) &&
       (sflt->ro.opts.side == opts_final.side) &&
       (sflt->ro.opts.thresh == opts_final.thresh) &&
       (sflt->ro.opts.size_cap == opts_final.size_cap) &&
       (sflt->ro.opts.side_shrink[0] == opts_final.side_shrink[0]) &&
       (sflt->ro.opts.side_shrink[1] == opts_final.side_shrink[1]))
    {
        return 0;
    }

    sflt->ro.opts = opts_final;

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

    const ntg_sidefloat* sflt = (const ntg_sidefloat*)ap;
    const ntg_widget* root = ctx->root;

    size_t base_pos = ctx->base_pos;

    struct ntg_widget_measure root_measure;
    root_measure = ntg_widget_get_measure(root, orient);

    const ntg_scene* scene = ntg_widget_get_scene(root);
    size_t scene_size = scene ? ntg_xy_get(scene->ro.size, orient) : 0;

    size_t base_size = ctx->base_size;
    if(base_size == 0) return 0;

    bool capped = (sflt->ro.opts.size_cap == NTG_SIDEFLOAT_SZCAP_ANCH);
    size_t side_shrink = sflt->ro.opts.side_shrink[0] + sflt->ro.opts.side_shrink[1];
    size_t available_space;

    switch(sflt->ro.opts.side)
    {
        case NTG_DIR_N:
            available_space = (orient == NTG_ORIENT_H) ?
                    (capped ? ntg_sub2_size(base_size, side_shrink) : scene_size) :
                    base_pos;
            break;
        case NTG_DIR_E:
            available_space = (orient == NTG_ORIENT_V) ?
                    (capped ? ntg_sub2_size(base_size, side_shrink) : scene_size) :
                    ntg_sub2_size(scene_size, base_size + base_pos);
            break;
        case NTG_DIR_S:
            available_space = (orient == NTG_ORIENT_H) ?
                    (capped ? ntg_sub2_size(base_size, side_shrink) : scene_size) :
                    ntg_sub2_size(scene_size, base_size + base_pos);
            break;
        case NTG_DIR_W:
            available_space = (orient == NTG_ORIENT_V) ?
                    (capped ? ntg_sub2_size(base_size, side_shrink) : scene_size) :
                    base_pos;
            break;
        default:
            return 0;
    }

    size_t thresh_size;
    switch(sflt->ro.opts.thresh)
    {
        case NTG_SIDEFLOAT_THRESH_MIN:
            thresh_size = root_measure.min_size;
            break;
        case NTG_SIDEFLOAT_THRESH_NAT:
            thresh_size = root_measure.nat_size;
            break;
        case NTG_SIDEFLOAT_THRESH_ALW:
            thresh_size = 0;
            break;
        default:
            return 0;
    }

    if(available_space < thresh_size)
        return 0;

    return ntg_min2_size(available_space, root_measure.nat_size);
}

static ntg_xy sidefloat_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    (void)arena;

    if(!ctx) return ntg_xy_new(0, 0);

    const ntg_sidefloat* sflt = (const ntg_sidefloat*)ap;

    ntg_xy pos = ntg_xy_new(0, 0);
    ntg_dxy size_diff = ntg_dxy_new(
            (ssize_t)ctx->size.ro.x - (ssize_t)ctx->base_size.ro.x,
            (ssize_t)ctx->size.ro.y - (ssize_t)ctx->base_size.ro.y);

    switch(sflt->ro.opts.side)
    {
        case NTG_DIR_N:
            pos = ntg_xy_set_x(pos, (size_diff.ro.x > 0) ?
                    ntg_sub2_size(ctx->base_pos.ro.x, size_diff.ro.x) :
                    ctx->base_pos.ro.x + ntg_align_offset_ssize(
                            ctx->size.ro.x,
                            ctx->base_size.ro.x,
                            sflt->ro.opts.align));
            pos = ntg_xy_set_y(
                    pos,
                    ntg_sub2_size(ctx->base_pos.ro.y, ctx->size.ro.y));
            break;
        case NTG_DIR_E:
            pos = ntg_xy_set_x(
                    pos,
                    ctx->base_pos.ro.x + ctx->base_size.ro.x);
            pos = ntg_xy_set_y(pos, (size_diff.ro.y > 0) ?
                    ntg_sub2_size(ctx->base_pos.ro.y, size_diff.ro.y) :
                    ctx->base_pos.ro.y + ntg_align_offset_ssize(
                            ctx->size.ro.y,
                            ctx->base_size.ro.y,
                            sflt->ro.opts.align));
            break;
        case NTG_DIR_S:
            pos = ntg_xy_set_x(pos, (size_diff.ro.x > 0) ?
                    ntg_sub2_size(ctx->base_pos.ro.x, size_diff.ro.x) :
                    ctx->base_pos.ro.x + ntg_align_offset_ssize(
                            ctx->size.ro.x,
                            ctx->base_size.ro.x,
                            sflt->ro.opts.align));
            pos = ntg_xy_set_y(
                    pos,
                    ctx->base_pos.ro.y + ctx->base_size.ro.y);
            break;
        case NTG_DIR_W:
            pos = ntg_xy_set_x(
                    pos,
                    ntg_sub2_size(ctx->base_pos.ro.x, ctx->size.ro.x));
            pos = ntg_xy_set_y(pos, (size_diff.ro.y > 0) ?
                    ntg_sub2_size(ctx->base_pos.ro.y, size_diff.ro.y) :
                    ctx->base_pos.ro.y + ntg_align_offset_ssize(
                            ctx->size.ro.y,
                            ctx->base_size.ro.y,
                            sflt->ro.opts.align));
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
