#ifndef NTG_SAP_H
#define NTG_SAP_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */


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
