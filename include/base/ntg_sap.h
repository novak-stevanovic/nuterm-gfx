#ifndef NTG_SAP_H
#define NTG_SAP_H

#include "thirdparty/sarena.h"
#include <stddef.h>

/* Generic Space Allocation Policies. */

/* Distributes `space_pool` space into `out_size_map`.
 * Distribution takes place depending on `caps` - if a size in `out_size_map`
 * reaches the capacity - it won't be awarded any more space.
 *
 * 'grows' decides how to allocate the `space_pool` - it doesn't take into
 * account already allocated space in `out_size_map`.
 *
 * If `grows` is NULL, all allocations will be treated equally.
 * 
 * RETURN VALUE: Total amount of distributed space. */
size_t ntg_sap_cap_round_robin(const size_t* caps, const size_t* grows,
                               size_t* out_size_map, size_t space_pool,
                               size_t count, sarena* arena);

#endif // NTG_SAP_H
