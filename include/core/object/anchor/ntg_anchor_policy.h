#ifndef NTG_ANCHOR_POLICY_H
#define NTG_ANCHOR_POLICY_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_anchor_policy
{
    const struct ntg_anchor_policy_vtable* __vtable;
    void* data;
};

NTG_API extern const ntg_anchor_policy NTG_ANCHOR_POLICY_ROOT;

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_anchor_policy_vdeinit(ntg_anchor_policy* ap);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PROTECTED */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_anchor_constrain_ctx
{
    const ntg_object* root;
    const ntg_object* base;
};

struct ntg_anchor_arrange_ctx
{
    const ntg_object* root;
    const ntg_object* base;
    struct ntg_xy size;
};

struct ntg_anchor_policy_vtable
{
    size_t (*constrain_fn)(
            const ntg_anchor_policy* ap,
            enum ntg_orient orient,
            const struct ntg_anchor_constrain_ctx* ctx,
            sarena* arena);
    struct ntg_xy (*arrange_fn)(
            const ntg_anchor_policy* ap,
            const struct ntg_anchor_arrange_ctx* ctx,
            sarena* arena);
    void (*deinit_fn)(ntg_anchor_policy* ap);
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_anchor_policy_init_inherit(
        ntg_anchor_policy* ap,
        const struct ntg_anchor_policy_vtable* vtable);

NTG_API int
ntg_anchor_policy_deinit(ntg_anchor_policy* ap);

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

size_t _ntg_anchor_policy_constrain(
        const ntg_anchor_policy* ap,
        enum ntg_orient orient,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

struct ntg_xy _ntg_anchor_policy_arrange(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

#endif // NTG_ANCHOR_POLICY_H
