#ifndef NTG_SAP_H
#define NTG_SAP_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* Distributes up to `space_pool` units among `count` entries without exceeding
 * `caps`. Allocation is round-robin and weighted by `grows`; a `NULL` `grows`
 * array gives every entry equal weight.
 *
 * RETURN VALUE:
 * The number of units added to `out_size_map`; `0` when no unit can be
 * assigned; `SIZE_MAX` on error.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `caps` or `out_size_map` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: temporary storage cannot be allocated from `arena`.
 * - `NTG_ERR_UNEXPECTED`: the calculated distribution would exceed a
 * capacity. */
NTG_API size_t
ntg_sap_cap_round_robin(
        const size_t* caps,
        const size_t* grows,
        size_t* out_size_map,
        size_t space_pool,
        size_t count,
        sarena* arena,
        int* out_status);

#endif // NTG_SAP_H
