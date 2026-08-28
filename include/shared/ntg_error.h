#ifndef NTG_ERROR_H
#define NTG_ERROR_H

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* ERROR CODES */
/* ========================================================================== */

#ifndef NTG_ERROR_BASE
#define NTG_ERROR_BASE 4000
#endif // NTG_ERROR_BASE

#define NTG_ERR_UNEXPECTED (NTG_ERROR_BASE + 1)
#define NTG_ERR_INV_ARG (NTG_ERROR_BASE + 2)
#define NTG_ERR_ALLOC_FAIL (NTG_ERROR_BASE + 3)
#define NTG_ERR_BAD_TYPE (NTG_ERROR_BASE + 4)
#define NTG_ERR_BAD_VTABLE (NTG_ERROR_BASE + 5)
#define NTG_ERR_OUT_OF_BOUNDS (NTG_ERROR_BASE + 6)
#define NTG_ERR_OVERFLOW (NTG_ERROR_BASE + 7)
#define NTG_ERR_UTF_CONV_FAIL (NTG_ERROR_BASE + 8)
#define NTG_ERR_NO_CAP (NTG_ERROR_BASE + 9)
#define NTG_ERR_THREAD_SPAWN (NTG_ERROR_BASE + 10)
#define NTG_ERR_INV_STATE (NTG_ERROR_BASE + 11)
#define NTG_ERR_MUTEX_INIT_FAIL (NTG_ERROR_BASE + 13)
#define NTG_ERR_COND_INIT_FAIL (NTG_ERROR_BASE + 14)
#define NTG_ERR_FOPEN_FAIL (NTG_ERROR_BASE + 15)

#define NTG_ERR_UNSUPP_TERM (NTG_ERROR_BASE + 100)
#define NTG_ERR_BAD_FCS_SCOPE (NTG_ERROR_BASE + 101)

#endif // NTG_ERROR_H
