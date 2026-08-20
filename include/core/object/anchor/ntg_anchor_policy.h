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
    struct
    {
        void* data;
    } pub;

    struct
    {
        const struct ntg_anchor_policy_vtable* vtable;
    } priv;
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

    size_t base_size, base_pos;
    size_t base_min_size, base_nat_size, base_max_size;
};

struct ntg_anchor_arrange_ctx
{
    struct ntg_xy size;
    const ntg_object* root;

    struct ntg_xy base_size, base_pos;
    struct ntg_xy base_min_size, base_nat_size, base_max_size;
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

size_t ntg__anchor_policy_hconstrain(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

size_t ntg__anchor_policy_vconstrain(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_constrain_ctx* ctx,
        sarena* arena);

struct ntg_xy ntg__anchor_policy_arrange(
        const ntg_anchor_policy* ap,
        const struct ntg_anchor_arrange_ctx* ctx,
        sarena* arena);

#endif // NTG_ANCHOR_POLICY_H
