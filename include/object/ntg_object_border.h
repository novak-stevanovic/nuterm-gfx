#ifndef _NTG_OBJECT_BORDER_H_
#define _NTG_OBJECT_BORDER_H_

#include "base/ntg_cell.h"

typedef struct ntg_object_drawing ntg_object_drawing;

struct ntg_object_border
{
    ntg_cell north_line, east_line,
             south_line, west_line;

    ntg_cell north_west_corner, north_east_corner,
             south_west_corner, south_east_corner;

    ntg_cell border_inside;
};

struct ntg_object_border ntg_object_border_no_border();

void ntg_object_border_apply(ntg_object_drawing* drawing,
        struct ntg_object_border border);

#endif // _NTG_OBJECT_BORDER_H_
