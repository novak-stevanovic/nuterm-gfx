#ifndef NTG_OBJECT_MEASURE_H
#define NTG_OBJECT_MEASURE_H

#include <stddef.h>

struct ntg_object_measure
{
    size_t min_size, natural_size, max_size, grow;
};

#endif // _NTG_OBJECT_MEASURE_H_
