#ifndef _NTG_OBJECT_MEASURE_H_
#define _NTG_OBJECT_MEASURE_H_

#include <stddef.h>

struct ntg_object_measure
{
    size_t min_size, natural_size, max_size, grow;
};

#endif // __NTG_OBJECT_MEASURE_H__
