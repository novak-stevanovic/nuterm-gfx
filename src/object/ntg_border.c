#include "object/ntg_border.h"
#include <assert.h>

struct ntg_border_style ntg_border_style_default()
{
    return (struct ntg_border_style) {

        .north_line = ntg_cell_default(),
        .east_line = ntg_cell_default(),
        .south_line = ntg_cell_default(),
        .west_line = ntg_cell_default(),

        .north_west_corner = ntg_cell_default(),
        .north_east_corner = ntg_cell_default(),
        .south_west_corner = ntg_cell_default(),
        .south_east_corner = ntg_cell_default(),

        .border_inside = ntg_cell_default(),
    };
}
