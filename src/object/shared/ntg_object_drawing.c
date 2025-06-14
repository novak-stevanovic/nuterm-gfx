#include <stdlib.h>
#include <assert.h>

#include "object/shared/ntg_object_drawing.h"

void __ntg_object_drawing_init__(ntg_object_drawing_t* drawing)
{
    if(drawing == NULL) return;

    __ntg_cell_grid_init__(&drawing->__data);
    drawing->__size = NTG_XY_UNSET;
}

void __ntg_object_drawing_deinit__(ntg_object_drawing_t* drawing)
{
    if(drawing == NULL) return;

    __ntg_cell_grid_deinit__(&drawing->__data);
    drawing->__size = NTG_XY_UNSET;
}

ntg_object_drawing_t* ntg_object_drawing_new()
{
    ntg_object_drawing_t* new = (ntg_object_drawing_t*)malloc(
            sizeof(struct ntg_object_drawing));

    if(new == NULL) return NULL;

    __ntg_object_drawing_init__(new);

    return new;
}

void ntg_object_drawing_destroy(ntg_object_drawing_t* drawing)
{
    if(drawing == NULL) return;

    __ntg_object_drawing_deinit__(drawing);

    free(drawing);
}

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing_t* drawing)
{
    return (drawing != NULL) ? drawing->__size : NTG_XY_UNSET;
}

void ntg_object_drawing_set_size(ntg_object_drawing_t* drawing,
        struct ntg_xy size)
{
    if(drawing == NULL) return;

    drawing->__size = size;

    ntg_status_t _status;
    ntg_cell_grid_set_size(&drawing->__data, size, &_status);
    assert(_status == NTG_SUCCESS);
}
