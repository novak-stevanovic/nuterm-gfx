#ifndef _NTG_BORDER_H_
#define _NTG_BORDER_H_

#include "base/ntg_cell.h"

struct ntg_border_style
{
    ntg_cell north_line, east_line,
             south_line, west_line;

    ntg_cell north_west_corner, north_east_corner,
             south_west_corner, south_east_corner;

    ntg_cell border_inside;
};

struct ntg_border_size
{
    size_t north, east, south, west;
};

static const struct ntg_border_size NTG_BORDER_SIZE_DEFAULT = {0};

static inline struct ntg_xy ntg_border_size_sum(struct ntg_border_size border_size)
{
    return ntg_xy(border_size.west + border_size.east,
            border_size.north + border_size.south);
}

static inline struct ntg_xy ntg_border_size_offset(struct ntg_border_size border_size)
{
    return ntg_xy(border_size.west, border_size.north);
}

struct ntg_border_style ntg_border_style_default();

#endif // _NTG_BORDER_H_
