#ifndef NTG_VECGRID_H
#define NTG_VECGRID_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

#define NTG_VECGRID_CAP_FACTOR_MIN 1.1

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_vecgrid
{
    void* data;
    struct ntg_xy size;
    size_t cap;
    double cap_factor;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_vecgrid_init(struct ntg_vecgrid* vecgrid, double cap_factor);

NTG_API int
ntg_vecgrid_deinit(struct ntg_vecgrid* vecgrid);

/* zero size can't fail */
NTG_API int
ntg_vecgrid_set_size(struct ntg_vecgrid* vecgrid, struct ntg_xy size, size_t data_size);

#endif // NTG_VECGRID_H
