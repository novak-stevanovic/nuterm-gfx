#ifndef NTG_ERROR_H
#define NTG_ERROR_H

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* ERROR CODES */
/* -------------------------------------------------------------------------- */

/* A function that accepts an `out_status` parameter reports success by
 * setting it to 0, and reports failure by setting it to the appropriate
 * error code. */

#define NTG_ERROR_BASE 4000

#define NTG_ERR_UNEXPECTED (NTG_ERROR_BASE + 1)
#define NTG_ERR_INVALID_ARG (NTG_ERROR_BASE + 2)
#define NTG_ERR_MAX_THREAD (NTG_ERROR_BASE + 3)
#define NTG_ERR_ALLOC_FAIL (NTG_ERROR_BASE + 4)
#define NTG_ERR_INVALID_TYPE (NTG_ERROR_BASE + 6)
#define NTG_ERR_BAD_VTABLE (NTG_ERROR_BASE + 7)
#define NTG_ERR_OUT_OF_BOUNDS (NTG_ERROR_BASE + 8)
#define NTG_ERR_UNSUPP_TERM (NTG_ERROR_BASE + 9)
#define NTG_ERR_LOG_INIT_FAIL (NTG_ERROR_BASE + 10)

#define NTG_ERR_STAGE_HAS_LOOP (NTG_ERROR_BASE + 100)

#define NTG_ERR_THREAD_SPAWN (NTG_ERROR_BASE + 101)

#define NTG_ERR_LOOP_RUNNING (NTG_ERROR_BASE + 102)

#define NTG_ERR_MAX_ANCHORED (NTG_ERROR_BASE + 103)
#define NTG_ERR_MAX_CHILDREN (NTG_ERROR_BASE + 104)

#define NTG_ERR_PLATFORM_NO_LOOP (NTG_ERROR_BASE + 105)

#define NTG_ERR_SCENE_EMPTY (NTG_ERROR_BASE + 107)
#define NTG_ERR_SCOPE_NOT_IN_SCENE (NTG_ERROR_BASE + 108)

#define NTG_ERR_UTF_CONV (NTG_ERROR_BASE + 200)

#endif // NTG_ERROR_H
