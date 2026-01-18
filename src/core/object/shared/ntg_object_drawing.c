#include "ntg.h"
#include <assert.h>
#include <stdlib.h>

void ntg_object_drawing_init(ntg_object_drawing* drawing)
{
    if(drawing == NULL) return;

    ntg_vcell_vecgrid_init(&drawing->__data);
}

void ntg_object_drawing_deinit(ntg_object_drawing* drawing)
{
    if(drawing == NULL) return;

    ntg_vcell_vecgrid_deinit(&drawing->__data);
}

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* drawing)
{
    return (drawing != NULL) ?
        ntg_vcell_vecgrid_get_size(&drawing->__data) :
        NTG_XY_UNSET;
}

void ntg_object_drawing_set_size(ntg_object_drawing* drawing,
    struct ntg_xy size, struct ntg_xy size_cap)
{
    if(drawing == NULL) return;

    ntg_vcell_vecgrid_set_size(&drawing->__data, size, size_cap);
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

    assert(ntg_xy_is_greater(src_size, src_start_pos));
    assert(ntg_xy_is_greater_eq(src_size, src_end_pos));
    assert(ntg_xy_is_greater(src_end_pos, src_start_pos));

    assert(ntg_xy_is_greater(dest_size, dest_start_pos));
    
    struct ntg_xy dest_end_pos = ntg_xy_add(dest_start_pos, src_box_size);

    assert(ntg_xy_is_greater_eq(dest_size, dest_end_pos));

    size_t i, j;
    struct ntg_xy it_dest_pos, it_src_pos;
    struct ntg_vcell* it_dest_cell;
    const struct ntg_vcell* it_src_cell;
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
        ntg_stage_drawing* dest_drawing, struct ntg_xy dest_start_pos)
{
    assert(src_drawing != NULL);
    assert(dest_drawing != NULL);

    src_box_size = ntg_xy_size(src_box_size);
    if(ntg_xy_is_zero(src_box_size)) return;

    struct ntg_xy dest_size = ntg_stage_drawing_get_size(dest_drawing);
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
    struct ntg_cell* it_dest_cell;
    const struct ntg_vcell* it_src_cell;
    for(i = 0; i < src_box_size.y; i++)
    {
        for(j = 0; j < src_box_size.x; j++)
        {
            it_dest_pos = ntg_xy_add(dest_start_pos, ntg_xy(j, i));
            it_src_pos = ntg_xy_add(src_start_pos, ntg_xy(j, i));

            it_dest_cell = ntg_stage_drawing_at_(dest_drawing, it_dest_pos);
            it_src_cell = ntg_object_drawing_at(src_drawing, it_src_pos);

            (*it_dest_cell) = ntg_vcell_overwrite(*it_src_cell, *it_dest_cell);
        }
    }
}

void ntg_tmp_object_drawing_init(ntg_tmp_object_drawing* drawing,
        struct ntg_xy size, sarena* arena)
{
    assert(drawing != NULL);
    assert(arena != NULL);

    if(ntg_xy_size_is_zero(size))
    {
        drawing->__data = NULL;
        drawing->_size = ntg_xy(0, 0);
    }
    else
    {
        drawing->__data = sarena_malloc(arena, sizeof(struct ntg_vcell) *
                size.x * size.y);
        assert(drawing->__data != NULL);

        drawing->_size = size;

        size_t i, j;
        struct ntg_vcell* it_cell;
        for(i = 0; i < size.y; i++)
        {
            for(j = 0; j < size.x; j++)
            {
                it_cell = ntg_tmp_object_drawing_at_(drawing, ntg_xy(j, i));
                (*it_cell) = ntg_vcell_default();
            }
        }
    }
}
