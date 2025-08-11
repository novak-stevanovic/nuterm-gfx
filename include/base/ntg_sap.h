#ifndef _NTG_SAP_H_
#define _NTG_SAP_H_

/* Generic Space Allocation Policies. */

#include <stddef.h>

/* Distributes `space_pool` space into `out_sizes`.
 * Distribution takes place depending on `caps` - if a size in `out_sizes`
 * reaches the capacity - it won't be awarded any more space.
 * 
 * RETURN VALUE: Total amount of distributed space. */
size_t ntg_sap_cap_round_robin(const size_t* caps, size_t* out_sizes,
        size_t space_pool, size_t count);

#endif // _NTG_SAP_H_
