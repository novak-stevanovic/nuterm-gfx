#ifndef __NTG_MEASURE_OUTPUT_H__
#define __NTG_MEASURE_OUTPUT_H__

#include <stddef.h>

struct ntg_measure_output
{
    // TODO: grow for objects
    size_t min_size, natural_size, max_size, grow;
};

#endif // __NTG_MEASURE_OUTPUT_H__
