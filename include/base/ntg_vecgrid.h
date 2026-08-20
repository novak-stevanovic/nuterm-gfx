#ifndef NTG_VECGRID_H
#define NTG_VECGRID_H

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

struct ntg_vecgrid
{
    struct
    {
        void* data;
        struct ntg_xy size;
        size_t capacity;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_vecgrid_init(ntg_vecgrid* vecgrid);

NTG_API int
ntg_vecgrid_deinit(ntg_vecgrid* vecgrid);

NTG_API int
ntg_vecgrid_set_size(
        ntg_vecgrid* vecgrid,
        struct ntg_xy size,
        double modifier,
        struct ntg_xy size_cap,
        size_t data_size);

#endif // NTG_VECGRID_H
