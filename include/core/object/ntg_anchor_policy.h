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

const struct ntg_anchor_policy* ntg_anchor_policy_root();

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

struct ntg_sidefloat_policy_opts ntg_sidefloat_policy_opts_def();

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

void ntg_anchor_policy_deinit(struct ntg_anchor_policy* policy);
void ntg_anchor_policy_deinit_(void* _policy);

void ntg_anchor_policy_init_float(
        struct ntg_anchor_policy* policy,
        const struct ntg_float_policy_opts* opts);

void ntg_anchor_policy_init_sidefloat(
        struct ntg_anchor_policy* policy,
        const struct ntg_sidefloat_policy_opts* opts);

#endif // NTG_ANCHOR_POLICY_H
