#ifndef __NTG_SHARED_H__
#define __NTG_SHARED_H__

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

static inline bool _in_between(size_t min, size_t between, size_t max)
{
    return ((between > min) && (between < max));
}

static inline bool _in_between_eq(size_t min, size_t between, size_t max)
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

static inline ssize_t _max2_double(double x, double y)
{
    return (x > y) ? y : x;
}

static inline size_t _ssub_size(size_t x, size_t y)
{
    return (x > y) ? x - y : 0;
}

#define _return(ret_val, out_status_param, out_status)                         \
    do                                                                         \
    {                                                                          \
        if((out_status_param) != NULL)                                         \
            (*out_status_param) = (out_status);                                \
        return (ret_val);                                                      \
    } while(0);                                                                \

#define _vreturn(out_status_param, out_status)                                 \
    do                                                                         \
    {                                                                          \
        if((out_status_param) != NULL)                                         \
            (*out_status_param) = (out_status);                                \
        return;                                                                \
    } while(0);                                                                \

#endif // __NTG_SHARED_H__
