#ifndef __NTG_OBJECT_MEASURE_H__
#define __NTG_OBJECT_MEASURE_H__

#include <stddef.h>

struct ntg_object_measure
{
    size_t min_size, natural_size, max_size, grow;
};

#endif // __NTG_OBJECT_MEASURE_H__
