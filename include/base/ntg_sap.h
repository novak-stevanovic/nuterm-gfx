#ifndef _NTG_SAP_H_
#define _NTG_SAP_H_

/* Generic Space Allocation Policies. */

#include <stddef.h>

/* sum of `nsizes` should be greater or equal to `space_pool` */
void ntg_sap_nsize_round_robin(const size_t* nsizes, size_t* out_sizes,
        size_t space_pool, size_t count);

#endif // _NTG_SAP_H_
