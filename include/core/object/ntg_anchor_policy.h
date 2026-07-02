#ifndef NTG_ANCHOR_POLICY_H
#define NTG_ANCHOR_POLICY_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* BASE */
/* -------------------------------------------------------------------------- */

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

struct ntg_anchor_policy
{
    size_t (*constrain_fn)(
            const void* data,
            ntg_orient orient,
            const struct ntg_anchor_constrain_ctx* ctx,
            sarena* arena);
    struct ntg_xy (*arrange_fn)(
            const void* data,
            const struct ntg_anchor_arrange_ctx* ctx,
            sarena* arena);
    void* data;
    void (*free_fn)(void* data);
};

/* -------------------------------------------------------------------------- */
/* FLOAT */
/* -------------------------------------------------------------------------- */

enum ntg_float_policy_enable
{
    NTG_FLOAT_POLICY_ENABLE_MIN,
    NTG_FLOAT_POLICY_ENABLE_BASE_MIN,
    NTG_FLOAT_POLICY_ENABLE_NAT,
    NTG_FLOAT_POLICY_ENABLE_BASE_NAT,
    NTG_FLOAT_POLICY_ENABLE_ALWAYS,
};

struct ntg_float_policy_opts
{
    enum ntg_float_policy_enable enable;
    struct ntg_insets shrink;
    ntg_align prim_align, sec_align;
};

/* Creates float-policy defaults: minimum enable mode, zero shrink insets, and start alignment on */
/* both axes. */

/* RETURN VALUE: The default `ntg_float_policy_opts` value. */
struct ntg_float_policy_opts ntg_float_policy_opts_def();

/* -------------------------------------------------------------------------- */
/* SIDE FLOAT */
/* -------------------------------------------------------------------------- */

enum ntg_sidefloat_policy_thresh
{
    NTG_SIDEFLOAT_POLICY_THRESH_MIN,
    NTG_SIDEFLOAT_POLICY_THRESH_NAT,
    NTG_SIDEFLOAT_POLICY_THRESH_ALWAYS,
};

enum ntg_sidefloat_policy_szcap
{
    NTG_SIDEFLOAT_POLICY_SZCAP_NONE,
    NTG_SIDEFLOAT_POLICY_SZCAP_ANCHOR
};

struct ntg_sidefloat_policy_opts
{
    ntg_align align;
    enum ntg_side side;
    enum ntg_sidefloat_policy_thresh thresh;
    enum ntg_sidefloat_policy_szcap size_cap;
};

/* Creates side-float defaults: start alignment, north side, minimum threshold, and no anchor */
/* size cap. */

/* RETURN VALUE: The default `ntg_sidefloat_policy_opts` value. */
struct ntg_sidefloat_policy_opts ntg_sidefloat_policy_opts_def();

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Calls the policy-specific data destructor, then clears the policy. Passing `NULL` has no */
/* effect. */
void ntg_anchor_policy_deinit(struct ntg_anchor_policy* policy);
/* Void-pointer adapter for `ntg_anchor_policy_deinit`, intended for cleanup callbacks. */
void ntg_anchor_policy_deinit_(void* _policy);

/* Gets the built-in policy that positions an anchored layer as the root layer. */

/* RETURN VALUE: A pointer to a process-lifetime, read-only root anchor policy. */
const struct ntg_anchor_policy* ntg_anchor_policy_root();

/* Initializes a policy that conditionally floats and aligns an anchored layer within its base. A */
/* `NULL` options pointer selects defaults. */

/* ERROR CODES: */
/* - `NTG_ERR_INVALID_ARG`: `policy` is `NULL`. */
/* - `NTG_ERR_ALLOC_FAIL`: policy-specific option storage cannot be allocated. */
void ntg_anchor_policy_init_float(
        struct ntg_anchor_policy* policy,
        const struct ntg_float_policy_opts* opts,
        int* out_status);

/* Initializes a policy that places an anchored layer along a selected side of its base. A `NULL` */
/* options pointer selects defaults. */

/* ERROR CODES: */
/* - `NTG_ERR_INVALID_ARG`: `policy` is `NULL`. */
/* - `NTG_ERR_ALLOC_FAIL`: policy-specific option storage cannot be allocated. */
void ntg_anchor_policy_init_sidefloat(
        struct ntg_anchor_policy* policy,
        const struct ntg_sidefloat_policy_opts* opts,
        int* out_status);

#endif // NTG_ANCHOR_POLICY_H
