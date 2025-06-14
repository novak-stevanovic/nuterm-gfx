#include <stdlib.h>
#include <assert.h>

#include "core/ntg_scene_drawing.h"

void __ntg_scene_drawing_init__(ntg_scene_drawing_t* drawing)
{
    if(drawing == NULL) return;

    __ntg_rcell_grid_init__(&drawing->__data);
    drawing->__size = NTG_XY_UNSET;
}

void __ntg_scene_drawing_deinit__(ntg_scene_drawing_t* drawing)
{
    if(drawing == NULL) return;

    __ntg_rcell_grid_deinit__(&drawing->__data);
    drawing->__size = NTG_XY_UNSET;
}

ntg_scene_drawing_t* ntg_scene_drawing_new()
{
    ntg_scene_drawing_t* new = (ntg_scene_drawing_t*)malloc(
            sizeof(struct ntg_scene_drawing));

    if(new == NULL) return NULL;

    __ntg_scene_drawing_init__(new);

    return new;
}

void ntg_scene_drawing_destroy(ntg_scene_drawing_t* drawing)
{
    if(drawing == NULL) return;

    __ntg_scene_drawing_deinit__(drawing);

    free(drawing);
}

struct ntg_xy ntg_scene_drawing_get_size(const ntg_scene_drawing_t* drawing)
{
    return (drawing != NULL) ? drawing->__size : NTG_XY_UNSET;
}

void ntg_scene_drawing_set_size(ntg_scene_drawing_t* drawing,
        struct ntg_xy size)
{
    if(drawing == NULL) return;

    drawing->__size = size;

    ntg_status_t _status;
    ntg_rcell_grid_set_size(&drawing->__data, size, &_status);
    assert(_status == NTG_SUCCESS);
}
