#ifndef _NTG_PADDING_H_
#define _NTG_PADDING_H_

#include <stddef.h>
#include "shared/ntg_xy.h"

struct ntg_padding_size
{
    size_t north, east, south, west;
};

static const struct ntg_padding_size NTG_BOX_PADDING_SIZE_ZERO = {0};

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

#endif // _NTG_PADDING_H_
