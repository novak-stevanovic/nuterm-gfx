#ifndef NTG_SIDEFLOAT_H
#define NTG_SIDEFLOAT_H

#include "shared/ntg_shared.h"
#include "core/widget/anchor/ntg_anchor_policy.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

enum ntg_sidefloat_thresh
{
    NTG_SIDEFLOAT_THRESH_MIN = 0,
    NTG_SIDEFLOAT_THRESH_NAT,
    NTG_SIDEFLOAT_THRESH_ALW,
};

enum ntg_sidefloat_size_cap
{
    NTG_SIDEFLOAT_SZCAP_NONE = 0,
    NTG_SIDEFLOAT_SZCAP_ANCH
};

struct ntg_sidefloat_opts
{
    enum ntg_align align;
    enum ntg_dir side;
    enum ntg_sidefloat_thresh thresh;
    enum ntg_sidefloat_size_cap size_cap;
    size_t side_shrink[2];
};

static const struct ntg_sidefloat_opts NTG_SIDEFLOAT_OPTS_ZERO = {0};

struct ntg_sidefloat
{
    ntg_anchor_policy _base;

    struct
    {
        struct ntg_sidefloat_opts opts;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_sidefloat_init(ntg_sidefloat* sflt, const struct ntg_sidefloat_opts* opts);

NTG_API int
ntg_sidefloat_deinit(ntg_sidefloat* sflt);

NTG_API int
ntg_sidefloat_set_opts(ntg_sidefloat* sflt, const struct ntg_sidefloat_opts* opts);

#endif // NTG_SIDEFLOAT_H
