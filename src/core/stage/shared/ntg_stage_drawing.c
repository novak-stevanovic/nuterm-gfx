#include "ntg.h"
#include <assert.h>
#include <stdlib.h>

void ntg_stage_drawing_init(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    ntg_cell_vecgrid_init(&drawing->___data);
}

void _ntg_stage_drawing_deinit_(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    ntg_cell_vecgrid_deinit(&drawing->___data);
}

ntg_stage_drawing* ntg_stage_drawing_new()
{
    ntg_stage_drawing* new = (ntg_stage_drawing*)malloc(
            sizeof(struct ntg_stage_drawing));

    if(new == NULL) return NULL;

    ntg_stage_drawing_init(new);

    return new;
}

void ntg_stage_drawing_destroy(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    _ntg_stage_drawing_deinit_(drawing);

    free(drawing);
}

struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing)
{
    return (drawing != NULL) ?
        ntg_cell_vecgrid_get_size(&drawing->___data) :
        NTG_XY_UNSET;
}

void ntg_stage_drawing_set_size(ntg_stage_drawing* drawing,
        struct ntg_xy size)
{
    if(drawing == NULL) return;

    ntg_status _status;
    ntg_cell_vecgrid_set_size(&drawing->___data, size, &_status);
    assert(_status == NTG_SUCCESS);
}
