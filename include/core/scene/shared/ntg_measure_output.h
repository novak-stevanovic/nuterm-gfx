#ifndef _NTG_MEASURE_OUTPUT_H_
#define _NTG_MEASURE_OUTPUT_H_

#include <stddef.h>

struct ntg_measure_output
{
    // TODO: grow for objects
    size_t min_size, natural_size, max_size, grow;
};

#endif // _NTG_MEASURE_OUTPUT_H_
