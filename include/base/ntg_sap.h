#ifndef NTG_SAP_H
#define NTG_SAP_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* Distributes up to `space_pool` units among `count` entries without allowing
 * an entry to exceed its corresponding `caps` value. `grows` contains the
 * relative grow weights; if it is NULL, every entry is treated as having a
 * grow weight of 1.
 *
 * `scratch_buffer` is caller-provided scratch memory. If `count` is greater
 * than 0, it must point to storage suitably aligned for `double` and large
 * enough for at least `count` elements of type `double`
 * (`count * sizeof(double)` bytes). Its initial contents are ignored and may
 * be overwritten. The buffer is used only for the duration of the call and
 * is not retained.
 *
 * `out_sizes` must point to an array of at least `count` elements. Its initial
 * values are the sizes already assigned to each entry. The function adds the
 * newly distributed units to those values.
 *
 * `out_distributed`, if non-NULL, receives the number of whole units added to
 * `out_sizes`.
 *
 * RETURN VALUE:
 * 0 on success;
 * NTG_ERR_INV_ARG if `caps` or `out_sizes` is NULL, or if `count` is greater
 * than 0 and `scratch_buffer` is NULL;
 * NTG_ERR_UNEXPECTED if an internal invariant is violated. */
NTG_API int
ntg_sap_cap_round_robin(
        const size_t* caps,
        const size_t* grows,
        size_t space_pool,
        size_t count,
        double* scratch_buffer,
        size_t* out_sizes,
        size_t* out_distributed);

#endif // NTG_SAP_H
