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


static inline size_t _clamp_size(size_t min, size_t mid, size_t max)
{
    if(mid < min) mid = min;
    else if(mid > max) mid = max;

    return mid;
}

static inline int _clamp_int(int min, int mid, int max)
{
    if(mid < min) mid = min;
    else if(mid > max) mid = max;

    return mid;
}

static inline unsigned long long _clamp_ull(
        unsigned long long min,
        unsigned long long mid,
        unsigned long long max)
{
    if(mid < min) mid = min;
    else if(mid > max) mid = max;

    return mid;
}

static inline bool _in_between_size(size_t min, size_t between, size_t max)
{
    return ((between > min) && (between < max));
}

static inline bool _in_between_eq_size(size_t min, size_t between, size_t max)
{
    return ((between >= min) && (between <= max));
}

static inline size_t _max2_size(size_t x, size_t y)
{
    return (x > y) ? x : y;
}

static inline size_t _min2_size(size_t x, size_t y)
{
    return (x > y) ? y : x;
}

static inline unsigned int _max2_uint(unsigned int x, unsigned int y)
{
    return (x > y) ? x : y;
}

static inline unsigned int _min2_uint(unsigned int x, unsigned int y)
{
    return (x > y) ? y : x;
}

static inline size_t _max3_size(size_t x, size_t y, size_t z)
{
    size_t tmp = _max2_size(x, y);

    return _max2_size(tmp, z);
}

static inline size_t _min3_size(size_t x, size_t y, size_t z)
{
    size_t tmp = _min2_size(x, y);

    return _min2_size(tmp, z);
}

static inline ssize_t _max2_ssize(ssize_t x, ssize_t y)
{
    return (x > y) ? x : y;
}


static inline ssize_t _min2_ssize(ssize_t x, ssize_t y)
{
    return (x > y) ? y : x;
}

static inline double _min2_double(double x, double y)
{
    return (x > y) ? y : x;
}


static inline double _max2_double(double x, double y)
{
    return (x < y) ? y : x;
}


static inline bool _double_are_eql(double a, double b)
{
    const double eps = 1e-9;
    double diff = a - b;

    return diff > -eps && diff < eps;
}

static inline size_t _sub2_size(size_t x, size_t y)
{
    return (x > y) ? x - y : 0;
}

static inline unsigned int _sub2_uint(unsigned int x, unsigned int y)
{
    return (x > y) ? x - y : 0;
}

static inline unsigned long long _sub2_ull(unsigned long long int x, unsigned long long int y)
{
    return (x > y) ? x - y : 0;
}

static inline size_t _sub3_size(size_t x, size_t y, size_t z)
{
    return ((x >= (y + z)) ? ((x - y) - z) : 0);
}

#define ntg_set_out(out_param, out_val) \
    if((out_param)) (*(out_param)) = (out_val);

#endif // NTG_SHARED_INTERNAL_H
