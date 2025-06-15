#ifndef __NTG_SHARED_H__
#define __NTG_SHARED_H__

#include <stddef.h>
#include <sys/types.h>

static inline size_t _max2_size(size_t x, size_t y)
{
    return (x > y) ? x : y;
}

static inline size_t _min2_size(size_t x, size_t y)
{
    return (x > y) ? y : x;
}

static inline ssize_t _max2_ssize(ssize_t x, ssize_t y)
{
    return (x > y) ? x : y;
}

static inline ssize_t _min2_ssize(ssize_t x, ssize_t y)
{
    return (x > y) ? y : x;
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
