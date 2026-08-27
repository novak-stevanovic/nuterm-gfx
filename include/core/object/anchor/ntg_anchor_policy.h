#ifndef NTG_ANCHOR_POLICY_H
#define NTG_ANCHOR_POLICY_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"
#include "base/entity/ntg_entity.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

struct ntg_anchor_policy
{
    ntg_entity _base;

    struct
    {
        void* data;
    } pub;
};

NTG_API extern const ntg_anchor_policy NTG_ANCHOR_POLICY_ROOT;

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
    struct ntg_entity_vtable base;

    size_t (*constrain_fn)(
            const ntg_anchor_policy* ap,
            enum ntg_orient orient,
            const struct ntg_anchor_constrain_ctx* ctx,
            sarena* arena);
    struct ntg_xy (*arrange_fn)(
            const ntg_anchor_policy* ap,
            const struct ntg_anchor_arrange_ctx* ctx,
            sarena* arena);
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_anchor_policy_init_inherit(
        ntg_anchor_policy* ap,
        const struct ntg_anchor_policy_vtable* vtable,
        const ntg_type* type);

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
