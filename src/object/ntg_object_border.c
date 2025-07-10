#include "object/ntg_object_border.h"
#include "object/shared/ntg_object_drawing.h"
#include <assert.h>

struct ntg_object_border_style ntg_object_border_no_border()
{
    return (struct ntg_object_border_style) {

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
