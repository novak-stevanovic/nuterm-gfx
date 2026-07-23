#ifndef NTG_VECGRID_H
#define NTG_VECGRID_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_vecgrid
{
    void* _data;
    struct ntg_xy _size;
    size_t _capacity;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* Initializes an empty untyped two-dimensional vector grid. */
NTG_API void
ntg_vecgrid_init(ntg_vecgrid* vecgrid);
/* Releases the grid storage and resets its size and capacity. Passing `NULL`
 * has no effect. */
NTG_API void
ntg_vecgrid_deinit(ntg_vecgrid* vecgrid);

/* Resizes an untyped grid of `data_size`-byte elements while preserving
 * overlapping storage. `modifier` controls growth, and `size_cap` caps only
 * extra reserved capacity; a requested size may exceed it.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `vecgrid` is `NULL`, `data_size` is zero, or
 *   `modifier <= 1.05`.
 * - `NTG_ERR_OUT_OF_BOUNDS`: a dimension product or byte allocation size would
 *   overflow `size_t`.
 * - `NTG_ERR_ALLOC_FAIL`: storage allocation fails. */
NTG_API void
ntg_vecgrid_set_size(
        ntg_vecgrid* vecgrid,
        struct ntg_xy size,
        double modifier,
        struct ntg_xy size_cap,
        size_t data_size,
        int* out_status);

#endif // NTG_VECGRID_H
