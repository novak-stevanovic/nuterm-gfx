#include <stdlib.h>
#include <assert.h>

#include "core/stage/shared/ntg_stage_drawing.h"

void __ntg_stage_drawing_init__(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    __ntg_rcell_vgrid_init__(&drawing->___data);
}

void __ntg_stage_drawing_deinit__(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    __ntg_rcell_vgrid_deinit__(&drawing->___data);
}

ntg_stage_drawing* ntg_stage_drawing_new()
{
    ntg_stage_drawing* new = (ntg_stage_drawing*)malloc(
            sizeof(struct ntg_stage_drawing));

    if(new == NULL) return NULL;

    __ntg_stage_drawing_init__(new);

    return new;
}

void ntg_stage_drawing_destroy(ntg_stage_drawing* drawing)
{
    if(drawing == NULL) return;

    __ntg_stage_drawing_deinit__(drawing);

    free(drawing);
}

struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing)
{
    return (drawing != NULL) ?
        ntg_rcell_vgrid_get_size(&drawing->___data) :
        NTG_XY_UNSET;
}

void ntg_stage_drawing_set_size(ntg_stage_drawing* drawing,
        struct ntg_xy size)
{
    if(drawing == NULL) return;

    ntg_status _status;
    ntg_rcell_vgrid_set_size(&drawing->___data, size, &_status);
    assert(_status == NTG_SUCCESS);
}
