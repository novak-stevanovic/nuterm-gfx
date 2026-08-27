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

static void root_deinit_fn(ntg_object* _ap);

static const struct ntg_anchor_policy_vtable ROOT_VTABLE = {
    .base.deinit_fn = root_deinit_fn,
    .constrain_fn = root_constrain_fn,
    .arrange_fn = root_arrange_fn
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
    ._base.ro.vtable = &ROOT_VTABLE.base,
    ._base.ro.type = &NTG_TYPE_ANCHOR_POLICY,
    .pub.data = NULL
};

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
        const struct ntg_anchor_policy_vtable* vtable,
        const ntg_type* type)
{
    if(!ap || !vtable || !type)
        return NTG_ERR_INV_ARG;

    if(!ntg_type_instanceof(type, &NTG_TYPE_ANCHOR_POLICY))
        return NTG_ERR_BAD_TYPE;

    int status = ntg_object_init_inherit(ntg_obj(ap), &vtable->base, type);
    NTG_POST_INHERIT_CHECK_VTABLE(status);

    ntg_object_zero(ap);

    return 0;
}

int ntg_anchor_policy_deinit(ntg_anchor_policy* ap)
{
    if(!ap) return NTG_ERR_INV_ARG;

    ntg_object_zero(ap);
    ntg_object_deinit(ntg_obj(ap));

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
    if(!ap || !ntg_ap_vtbl(ap) || !ntg_ap_vtbl(ap)->constrain_fn)
        return 0;

    return ntg_ap_vtbl(ap)->constrain_fn(ap, NTG_ORIENT_H, ctx, arena);
}

size_t ntg__anchor_policy_vconstrain(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    if(!ap || !ntg_ap_vtbl(ap) || !ntg_ap_vtbl(ap)->constrain_fn)
        return 0;

    return ntg_ap_vtbl(ap)->constrain_fn(ap, NTG_ORIENT_V, ctx, arena);
}

struct ntg_xy ntg__anchor_policy_arrange(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    if(!ap || !ntg_ap_vtbl(ap) || !ntg_ap_vtbl(ap)->arrange_fn)
        return ntg_xy(0, 0);

    return ntg_ap_vtbl(ap)->arrange_fn(ap, ctx, arena);
}

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static void root_deinit_fn(ntg_object* _ap)
{
    ntg_anchor_policy_deinit(ntg_ap(_ap));
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

    const ntg_scene* scene = ntg_widget_get_scene(ctx->root);
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
