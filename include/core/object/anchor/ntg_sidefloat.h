#ifndef NTG_SIDEFLOAT_H
#define NTG_SIDEFLOAT_H

#include "shared/ntg_shared.h"
#include "core/object/anchor/ntg_anchor_policy.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

enum ntg_sidefloat_thresh
{
    NTG_SIDEFLOAT_THRESH_MIN,
    NTG_SIDEFLOAT_THRESH_NAT,
    NTG_SIDEFLOAT_THRESH_ALWAYS,
};

enum ntg_sidefloat_size_cap
{
    NTG_SIDEFLOAT_SIZE_CAP_NONE,
    NTG_SIDEFLOAT_SIZE_CAP_ANCHOR
};

struct ntg_sidefloat_opts
{
    ntg_align align;
    enum ntg_side side;
    enum ntg_sidefloat_thresh thresh;
    enum ntg_sidefloat_size_cap size_cap;
};

NTG_API struct ntg_sidefloat_opts ntg_sidefloat_opts_default();

struct ntg_sidefloat
{
    ntg_anchor_policy __base;
    struct ntg_sidefloat_opts _opts;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

NTG_API void
ntg_sidefloat_init(
        ntg_sidefloat* sidefloat_ap,
        const struct ntg_sidefloat_opts* opts,
        int* out_status);

NTG_API void
ntg_sidefloat_deinit(ntg_sidefloat* sidefloat_ap);

NTG_API void
ntg_sidefloat_deinit_void(void* _sidefloat_ap);

#endif // NTG_SIDEFLOAT_H
