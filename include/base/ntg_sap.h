#ifndef _NTG_SAP_H_
#define _NTG_SAP_H_

/* Generic Space Allocation Policies. */

#include <stddef.h>

typedef struct sarena sarena;

/* Distributes `space_pool` space into `out_sizes`.
 * Distribution takes place depending on `caps` - if a size in `out_sizes`
 * reaches the capacity - it won't be awarded any more space.
 *
 * 'grows' decides how to allocate the `space_pool` - it doesn't take into
 * account already allocated space in `out_sizes`.
 *
 * If `grows` is NULL, all allocations will be treated equally.
 * 
 * RETURN VALUE: Total amount of distributed space. */
size_t ntg_sap_cap_round_robin(const size_t* caps, const size_t* grows,
        size_t* out_sizes, size_t space_pool, size_t count, sarena* arena);

#endif // _NTG_SAP_H_
