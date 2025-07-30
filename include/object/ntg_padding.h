#ifndef _NTG_PADDING_H_
#define _NTG_PADDING_H_

#include <stddef.h>
#include <string.h>
#include "shared/ntg_xy.h"

struct ntg_padding_size
{
    size_t north, east, south, west;
};

static const struct ntg_padding_size NTG_PADDING_SIZE_ZERO = {0};

static inline struct ntg_xy ntg_padding_size_sum(struct ntg_padding_size padding)
{
    return ntg_xy(padding.west + padding.east,
            padding.north + padding.south);
}

static inline struct ntg_xy ntg_padding_size_offset(struct ntg_padding_size padding)
{
    return ntg_xy(padding.west, padding.north);
}

static inline size_t ntg_padding_size_y(struct ntg_padding_size padding)
{
    return (padding.north + padding.south);
}

static inline size_t ntg_padding_size_x(struct ntg_padding_size padding)
{
    return (padding.west + padding.east);
}

static inline bool ntg_padding_size_are_equal(struct ntg_padding_size p1,
        struct ntg_padding_size p2)
{
    return (memcmp(&p1, &p2, sizeof(struct ntg_padding_size)) == 0);
}

#endif // _NTG_PADDING_H_
