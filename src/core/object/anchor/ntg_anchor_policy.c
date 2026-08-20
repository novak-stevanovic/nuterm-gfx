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

static size_t root_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy root_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

static void root_deinit_fn(ntg_anchor_policy* ap);

static const struct ntg_anchor_policy_vtable ROOT_VTABLE = {
    .constrain_fn = root_constrain_fn,
    .arrange_fn = root_arrange_fn,
    .deinit_fn = root_deinit_fn
};

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

const ntg_anchor_policy NTG_ANCHOR_POLICY_ROOT = {
    .priv.vtable = &ROOT_VTABLE,
    .pub.data = NULL
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_anchor_policy_vdeinit(ntg_anchor_policy* ap)
{
    if(!ap) return NTG_ERR_INV_ARG;
    if(!ap->priv.vtable) return 0;

    if(ap->priv.vtable->deinit_fn)
        ap->priv.vtable->deinit_fn(ap);

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_anchor_policy_init_inherit(
        ntg_anchor_policy* ap,
        const struct ntg_anchor_policy_vtable* vtable)
{
    if(!ap)
        return NTG_ERR_INV_ARG;

    if(!vtable || !vtable->deinit_fn)
        return NTG_ERR_BAD_VTABLE;

    (*ap) = (ntg_anchor_policy) {
        .priv.vtable = vtable,
        .pub.data = NULL
    };
    return 0;
}

int ntg_anchor_policy_deinit(ntg_anchor_policy* ap)
{
    if(!ap) return NTG_ERR_INV_ARG;

    (*ap) = (ntg_anchor_policy) {0};

    return 0;
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

size_t ntg__anchor_policy_hconstrain(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    if(!ap || !ap->priv.vtable || !ap->priv.vtable->constrain_fn)
        return 0;

    return ap->priv.vtable->constrain_fn(ap, NTG_ORIENT_H, ctx, arena);
}

size_t ntg__anchor_policy_vconstrain(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    if(!ap || !ap->priv.vtable || !ap->priv.vtable->constrain_fn)
        return 0;

    return ap->priv.vtable->constrain_fn(ap, NTG_ORIENT_V, ctx, arena);
}

struct ntg_xy ntg__anchor_policy_arrange(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    if(!ap || !ap->priv.vtable || !ap->priv.vtable->arrange_fn)
        return ntg_xy(0, 0);

    return ap->priv.vtable->arrange_fn(ap, ctx, arena);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void root_deinit_fn(ntg_anchor_policy* ap)
{
    ntg_anchor_policy_deinit(ap);
}

static size_t root_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    (void)ap;
    (void)arena;
    (void)orient;

    if(!ctx || !ctx->root) return 0;

    const ntg_scene* scene = ntg_object_get_scene(ctx->root);
    if(!scene) return 0;

    return ctx->base_size;
}

static struct ntg_xy root_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    (void)ap;
    (void)ctx;
    (void)arena;

    return ctx->base_pos;
}
