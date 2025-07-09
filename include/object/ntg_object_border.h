#ifndef _NTG_OBJECT_BORDER_H_
#define _NTG_OBJECT_BORDER_H_

#include "base/ntg_cell.h"

struct ntg_object_border
{
    size_t pref_width;

    ntg_cell north_line, east_line,
             south_line, west_line;

    ntg_cell north_west_corner, north_east_corner,
             south_west_corner, south_east_corner;

    ntg_cell border_inside;
    bool border_inside_as_outline;
};

struct ntg_object_border ntg_object_border_no_border();

#endif // _NTG_OBJECT_BORDER_H_
