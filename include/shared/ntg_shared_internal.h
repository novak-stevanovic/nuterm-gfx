#ifndef NTG_SHARED_INTERNAL_H
#define NTG_SHARED_INTERNAL_H

#include "ntg.h"
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

#define ntg_set_out(out_param, out_val) \
    if((out_param)) (*(out_param)) = (out_val);

#define ntg_not_null(param) \
    if(!param) { return NTG_ERR_INV_ARG; }

#define ntg_set_deref(var, ptr_val) \
    if(ptr_val) var = (* ptr_val );

#define NTG_POST_INHERIT_CHECK_VTABLE(status) \
    switch(status) \
    { \
        case 0: \
            break; \
        case NTG_ERR_ALLOC_FAIL: \
            return NTG_ERR_ALLOC_FAIL; \
        case NTG_ERR_BAD_VTABLE: \
            return NTG_ERR_BAD_VTABLE; \
        default: \
            return NTG_ERR_UNEXPECTED; \
    } \
    
#define NTG_POST_INHERIT_CHECK(status) \
    switch(status) \
    { \
        case 0: \
            break; \
        case NTG_ERR_ALLOC_FAIL: \
            return NTG_ERR_ALLOC_FAIL; \
        default: \
            return NTG_ERR_UNEXPECTED; \
    } \

#endif // NTG_SHARED_INTERNAL_H
