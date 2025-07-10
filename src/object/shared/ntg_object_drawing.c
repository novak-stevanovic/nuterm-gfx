#include <stdlib.h>
#include <assert.h>

#include "object/shared/ntg_object_drawing.h"
#include "core/ntg_scene_drawing.h"
#include "shared/ntg_log.h"

void __ntg_object_drawing_init__(ntg_object_drawing* drawing)
{
    if(drawing == NULL) return;

    __ntg_cell_vgrid_init__(&drawing->___data);
}

void __ntg_object_drawing_deinit__(ntg_object_drawing* drawing)
{
    if(drawing == NULL) return;

    __ntg_cell_vgrid_deinit__(&drawing->___data);
}

ntg_object_drawing* ntg_object_drawing_new()
{
    ntg_object_drawing* new = (ntg_object_drawing*)malloc(
            sizeof(struct ntg_object_drawing));

    if(new == NULL) return NULL;

    __ntg_object_drawing_init__(new);

    return new;
}

void ntg_object_drawing_destroy(ntg_object_drawing* drawing)
{
    if(drawing == NULL) return;

    __ntg_object_drawing_deinit__(drawing);

    free(drawing);
}

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* drawing)
{
    return (drawing != NULL) ? ntg_cell_vgrid_get_size(&drawing->___data) : NTG_XY_UNSET;
}

void ntg_object_drawing_set_size(ntg_object_drawing* drawing,
        struct ntg_xy size)
{
    if(drawing == NULL) return;

    ntg_status _status;
    ntg_cell_vgrid_set_size(&drawing->___data, size, &_status);
    assert(_status == NTG_SUCCESS);
}

void ntg_object_drawing_place(const ntg_object_drawing* src_drawing,
        struct ntg_xy src_start_pos, struct ntg_xy src_box_size,
        ntg_object_drawing* dest_drawing, struct ntg_xy dest_start_pos)
{
    assert(src_drawing != NULL);
    assert(dest_drawing != NULL);
    assert(src_drawing != dest_drawing);

    src_box_size = ntg_xy_size(src_box_size);
    if(ntg_xy_is_zero(src_box_size)) return;

    struct ntg_xy dest_size = ntg_object_drawing_get_size(dest_drawing);
    struct ntg_xy src_size = ntg_object_drawing_get_size(src_drawing);

    struct ntg_xy src_end_pos = ntg_xy_add(src_start_pos, src_box_size);

    ntg_log_log("A: (%d,%d), (%d,%d)",
            src_size.x, src_size.y,
            dest_size.x, dest_size.y);

    ntg_log_log("B :(%d,%d), (%d,%d)",
            src_start_pos.x, src_start_pos.y,
            src_box_size.x, src_box_size.y);

    assert(ntg_xy_is_greater(src_size, src_start_pos));
    assert(ntg_xy_is_greater_eq(src_size, src_end_pos));
    assert(ntg_xy_is_greater(src_end_pos, src_start_pos));

    assert(ntg_xy_is_greater(dest_size, dest_start_pos));
    
    struct ntg_xy dest_end_pos = ntg_xy_add(dest_start_pos, src_box_size);

    ntg_log_log("C: (%d,%d), (%d,%d)",
            dest_start_pos.x, dest_start_pos.y,
            dest_end_pos.x, dest_end_pos.y);

    assert(ntg_xy_is_greater_eq(dest_size, dest_end_pos));

    size_t i, j;
    struct ntg_xy it_dest_pos, it_src_pos;
    ntg_cell* it_dest_cell;
    const ntg_cell* it_src_cell;
    for(i = 0; i < src_box_size.y; i++)
    {
        for(j = 0; j < src_box_size.x; j++)
        {
            it_dest_pos = ntg_xy_add(dest_start_pos, ntg_xy(j, i));
            it_src_pos = ntg_xy_add(src_start_pos, ntg_xy(j, i));

            it_dest_cell = ntg_object_drawing_at_(dest_drawing, it_dest_pos);
            it_src_cell = ntg_object_drawing_at(src_drawing, it_src_pos);

            (*it_dest_cell) = *it_src_cell;
        }
    }
}

void ntg_object_drawing_place_(const ntg_object_drawing* src_drawing,
        struct ntg_xy src_start_pos, struct ntg_xy src_box_size,
        ntg_scene_drawing* dest_drawing, struct ntg_xy dest_start_pos)
{
    assert(src_drawing != NULL);
    assert(dest_drawing != NULL);

    src_box_size = ntg_xy_size(src_box_size);
    if(ntg_xy_is_zero(src_box_size)) return;

    struct ntg_xy dest_size = ntg_scene_drawing_get_size(dest_drawing);
    struct ntg_xy src_size = ntg_object_drawing_get_size(src_drawing);

    struct ntg_xy src_end_pos = ntg_xy_add(src_start_pos, src_box_size);

    assert(ntg_xy_is_greater(src_size, src_start_pos));
    assert(ntg_xy_is_greater_eq(src_size, src_end_pos));
    assert(ntg_xy_is_greater(src_end_pos, src_start_pos));

    assert(ntg_xy_is_greater(dest_size, dest_start_pos));
    
    struct ntg_xy dest_end_pos = ntg_xy_add(dest_start_pos, src_box_size);

    assert(ntg_xy_is_greater_eq(dest_size, dest_end_pos));

    size_t i, j;
    struct ntg_xy it_dest_pos, it_src_pos;
    struct ntg_rcell* it_dest_cell;
    const ntg_cell* it_src_cell;
    for(i = 0; i < src_box_size.y; i++)
    {
        for(j = 0; j < src_box_size.x; j++)
        {
            it_dest_pos = ntg_xy_add(dest_start_pos, ntg_xy(j, i));
            it_src_pos = ntg_xy_add(src_start_pos, ntg_xy(j, i));

            it_dest_cell = ntg_scene_drawing_at_(dest_drawing, it_dest_pos);
            it_src_cell = ntg_object_drawing_at(src_drawing, it_src_pos);

            (*it_dest_cell) = ntg_cell_overwrite(*it_src_cell, *it_dest_cell);
        }
    }
}
