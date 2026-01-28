#ifndef NTG_OBJECT_MEASURE_H
#define NTG_OBJECT_MEASURE_H

#include <stddef.h>
#include <stdbool.h>

struct ntg_object_measure
{
    size_t min_size, nat_size, max_size, grow;
};

static inline bool ntg_object_measure_are_equal(
        struct ntg_object_measure m1,
        struct ntg_object_measure m2)
{
    return ((m1.min_size == m2.min_size) &&
    (m1.nat_size == m2.nat_size) &&
    (m1.max_size == m2.max_size) &&
    (m1.grow == m2.grow));
}

#endif // _NTG_OBJECT_MEASURE_H_
