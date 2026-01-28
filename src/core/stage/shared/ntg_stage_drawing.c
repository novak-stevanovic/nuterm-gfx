#include "ntg.h"
#include <assert.h>
#include <stdlib.h>

void ntg_stage_drawing_init(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    ntg_cell_vecgrid_init(&drawing->__data);
}

void ntg_stage_drawing_deinit(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    ntg_cell_vecgrid_deinit(&drawing->__data);
}

struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing)
{
    return (drawing != NULL) ?
        ntg_cell_vecgrid_get_size(&drawing->__data) :
        NTG_XY_UNSET;
}

void ntg_stage_drawing_set_size(ntg_stage_drawing* drawing,
    struct ntg_xy size, struct ntg_xy size_cap)
{
    if(drawing == NULL) return;

    ntg_cell_vecgrid_set_size(&drawing->__data, size, size_cap);
}
