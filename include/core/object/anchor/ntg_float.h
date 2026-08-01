#ifndef NTG_FLOAT_H
#define NTG_FLOAT_H

#include "shared/ntg_shared.h"
#include "core/object/anchor/ntg_anchor_policy.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

enum ntg_float_enable
{
    NTG_FLOAT_ENABLE_MIN,
    NTG_FLOAT_ENABLE_BASE_MIN,
    NTG_FLOAT_ENABLE_NAT,
    NTG_FLOAT_ENABLE_BASE_NAT,
    NTG_FLOAT_ENABLE_ALWAYS,
};

struct ntg_float_opts
{
    enum ntg_float_enable enable;
    struct ntg_insets shrink;
    ntg_align prim_align, sec_align;
};

NTG_API struct ntg_float_opts
ntg_float_opts_default();

struct ntg_float
{
    ntg_anchor_policy __base;
    struct ntg_float_opts _opts;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_float_init(
        ntg_float* float_ap,
        const struct ntg_float_opts* opts,
        int* out_status);

NTG_API void
ntg_float_deinit(ntg_float* float_ap);

NTG_API void
ntg_float_deinit_void(void* _float_ap);

#endif // NTG_FLOAT_H
