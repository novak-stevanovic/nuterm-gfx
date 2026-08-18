#include "ntg.h"
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static size_t root_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

static struct ntg_xy root_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

static const struct ntg_anchor_policy_vtable ROOT_VTABLE = {
    .constrain_fn = root_constrain_fn,
    .arrange_fn = root_arrange_fn,
    .deinit_fn = ntg_anchor_policy_deinit
};

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

const ntg_anchor_policy NTG_ANCHOR_POLICY_ROOT = {
    .__vtable = &ROOT_VTABLE,
    .data = NULL
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_anchor_policy_vdeinit(ntg_anchor_policy* ap)
{
    if(!ap || !ap->__vtable) return;

    if(ap->__vtable->deinit_fn)
        ap->__vtable->deinit_fn(ap);
}

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

void ntg_anchor_policy_init_inherit(
        ntg_anchor_policy* ap,
        const struct ntg_anchor_policy_vtable* vtable,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!ap)
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(!vtable || !vtable->deinit_fn)
        ntg_vreturn(out_status, NTG_ERR_BAD_VTABLE);

    (*ap) = (ntg_anchor_policy) {
        .__vtable = vtable,
        .data = NULL
    };
}

void ntg_anchor_policy_deinit(ntg_anchor_policy* ap)
{
    if(!ap) return;

    (*ap) = (ntg_anchor_policy) {0};
}

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

size_t _ntg_anchor_policy_constrain(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    if(!ap || !ap->__vtable || !ap->__vtable->constrain_fn)
        return 0;

    return ap->__vtable->constrain_fn(ap, orient, ctx, arena);
}

struct ntg_xy _ntg_anchor_policy_arrange(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    if(!ap || !ap->__vtable || !ap->__vtable->arrange_fn)
        return ntg_xy(0, 0);

    return ap->__vtable->arrange_fn(ap, ctx, arena);
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

static size_t root_constrain_fn(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena)
{
    (void)ap;
    (void)arena;

    if(!ctx || !ctx->root) return 0;

    const ntg_scene* scene = ntg_object_get_scene(ctx->root);
    if(!scene) return 0;

    return ntg_xy_get(scene->_size, orient);
}

static struct ntg_xy root_arrange_fn(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena)
{
    (void)ap;
    (void)ctx;
    (void)arena;

    return ntg_xy(0, 0);
}
