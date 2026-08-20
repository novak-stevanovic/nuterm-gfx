#ifndef NTG_FLOAT_H
#define NTG_FLOAT_H

#include "shared/ntg_shared.h"
#include "core/object/anchor/ntg_anchor_policy.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

enum ntg_float_enable
{
    NTG_FLOAT_ENABLE_MIN = 0,
    NTG_FLOAT_ENABLE_BASE_MIN,
    NTG_FLOAT_ENABLE_NAT,
    NTG_FLOAT_ENABLE_BASE_NAT,
    NTG_FLOAT_ENABLE_ALWAYS,
};

struct ntg_float_opts
{
    enum ntg_float_enable enable;
    struct ntg_insets shrink;
    enum ntg_align prim_align, sec_align;
};

NTG_API struct ntg_float_opts
ntg_float_opts_default(void);

struct ntg_float
{
    struct
    {
        ntg_anchor_policy base;
    } priv;

    struct
    {
        struct ntg_float_opts opts;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_float_init(ntg_float* float_ap, const struct ntg_float_opts* opts);

NTG_API int
ntg_float_deinit(ntg_float* float_ap);

NTG_API void
ntg_float_deinit_void(void* _float_ap);

#endif // NTG_FLOAT_H
