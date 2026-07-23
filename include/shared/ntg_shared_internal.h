#ifndef NTG_SHARED_INTERNAL_H
#define NTG_SHARED_INTERNAL_H

#include "ntg.h"
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* Clamps `mid` to the inclusive range from `min` to `max`.
 *
 * RETURN VALUE:
 * The clamped value. */
static inline size_t _clamp_size(size_t min, size_t mid, size_t max)
{
    if(mid < min) mid = min;
    else if(mid > max) mid = max;

    return mid;
}

/* Clamps `mid` to the inclusive range from `min` to `max`.
 *
 * RETURN VALUE:
 * The clamped value. */
static inline int _clamp_int(int min, int mid, int max)
{
    if(mid < min) mid = min;
    else if(mid > max) mid = max;

    return mid;
}

/* Checks whether `between` lies strictly inside the open interval `(min,
 * max)`. */
static inline bool _in_between(size_t min, size_t between, size_t max)
{
    return ((between > min) && (between < max));
}

/* Checks whether `between` lies in the closed interval `[min, max]`. */
static inline bool _in_between_eq(size_t min, size_t between, size_t max)
{
    return ((between >= min) && (between <= max));
}

/* Returns the larger of two `size_t` values.
 *
 * RETURN VALUE:
 * The maximum value. */
static inline size_t _max2_size(size_t x, size_t y)
{
    return (x > y) ? x : y;
}

/* Returns the smaller of two `size_t` values.
 *
 * RETURN VALUE:
 * The minimum value. */
static inline size_t _min2_size(size_t x, size_t y)
{
    return (x > y) ? y : x;
}

/* Returns the larger of two unsigned integers.
 *
 * RETURN VALUE:
 * The maximum value. */
static inline unsigned int _max2_uint(unsigned int x, unsigned int y)
{
    return (x > y) ? x : y;
}

/* Returns the smaller of two unsigned integers.
 *
 * RETURN VALUE:
 * The minimum value. */
static inline unsigned int _min2_uint(unsigned int x, unsigned int y)
{
    return (x > y) ? y : x;
}

/* Returns the largest of three `size_t` values.
 *
 * RETURN VALUE:
 * The maximum value. */
static inline size_t _max3_size(size_t x, size_t y, size_t z)
{
    size_t tmp = _max2_size(x, y);

    return _max2_size(tmp, z);
}

/* Returns the smallest of three `size_t` values.
 *
 * RETURN VALUE:
 * The minimum value. */
static inline size_t _min3_size(size_t x, size_t y, size_t z)
{
    size_t tmp = _min2_size(x, y);

    return _min2_size(tmp, z);
}

/* Returns the larger of two `ssize_t` values.
 *
 * RETURN VALUE:
 * The maximum value. */
static inline ssize_t _max2_ssize(ssize_t x, ssize_t y)
{
    return (x > y) ? x : y;
}

/* Returns the smaller of two `ssize_t` values.
 *
 * RETURN VALUE:
 * The minimum value. */
static inline ssize_t _min2_ssize(ssize_t x, ssize_t y)
{
    return (x > y) ? y : x;
}

/* Returns the smaller of two `double` values.
 *
 * RETURN VALUE:
 * The minimum value. */
static inline double _min2_double(double x, double y)
{
    return (x > y) ? y : x;
}

/* Returns the larger of two `double` values.
 *
 * RETURN VALUE:
 * The maximum value. */
static inline double _max2_double(double x, double y)
{
    return (x < y) ? y : x;
}

/* Compares two floating-point values using the library epsilon tolerance. */
static inline bool _double_are_eql(double a, double b)
{
    const double eps = 1e-9;
    double diff = a - b;

    return diff > -eps && diff < eps;
}

/* Subtracts `y` from `x` with saturation at zero.
 *
 * RETURN VALUE:
 * `x - y` when `x > y`; otherwise `0`. */
static inline size_t _ssub_size(size_t x, size_t y)
{
    return (x > y) ? x - y : 0;
}

// static inline void _log_tmp_drawing(
//         const char* name,
//         ntg_object_tmp_drawing* drawing)
// {
//     size_t i, j;
//     char text[NTG_SIZE_MAX * 3][NTG_SIZE_MAX * 3];
//     for(i = 0; i < drawing->size.y; i++)
//     {
//         for(j = 0; j < drawing->size.x; j++)
//         {
//             text[i][j]
//         }
//     }
// }

#define ntg_return(ret_val, out_status_param, out_status)                      \
    do                                                                         \
    {                                                                          \
        if((out_status_param) != NULL)                                         \
            (*out_status_param) = (out_status);                                \
        return (ret_val);                                                      \
    } while(0);

#define ntg_vreturn(out_status_param, out_status)                              \
    do                                                                         \
    {                                                                          \
        if((out_status_param) != NULL)                                         \
            (*out_status_param) = (out_status);                                \
        return;                                                                \
    } while(0);

#define ntg_init_status(out_status_param) \
    if((out_status_param)) (*(out_status_param)) = 0;

#define ntg_set_status(out_status_param, out_status) \
    if((out_status_param)) (*(out_status_param)) = (out_status);

#endif // NTG_SHARED_INTERNAL_H
