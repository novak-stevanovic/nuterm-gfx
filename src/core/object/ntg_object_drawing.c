#include "ntg.h"
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

void ntg_object_drawing_init(ntg_object_drawing* drawing)
{
    if(!drawing) return;

    ntg_vcell_vecgrid_init(&drawing->__data);
}

void ntg_object_drawing_deinit(ntg_object_drawing* drawing)
{
    if(!drawing) return;

    ntg_vcell_vecgrid_deinit(&drawing->__data);
}

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* drawing)
{
    return (drawing != NULL) ?
        ntg_vcell_vecgrid_get_size(&drawing->__data) :
        NTG_XY_UNSET;
}

void ntg_object_drawing_set_size(
        ntg_object_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!drawing)
    {
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);
    }

    int _status;
    ntg_vcell_vecgrid_set_size(&drawing->__data, size, size_cap, &_status);

    switch(_status)
    {
        case NTG_SUCCESS:
            break;
        case NTG_ERR_ALLOC_FAIL:
            ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);
        default:
            ntg_vreturn(out_status, NTG_ERR_UNEXPECTED);
    }
}

void ntg_object_drawing_place(
        const ntg_object_drawing* src_drawing,
        ntg_object_drawing* dest_drawing,
        struct ntg_xy dest_start_pos,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!src_drawing || !dest_drawing || (src_drawing == dest_drawing))
    {
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);
    }

    struct ntg_xy dest_size = ntg_object_drawing_get_size(dest_drawing);
    struct ntg_xy src_size = ntg_object_drawing_get_size(src_drawing);

    if(ntg_xy_size_is_zero(src_size)) return; // success, nothing to draw

    struct ntg_xy dest_end_pos = ntg_xy_add(dest_start_pos, src_size);

    if(ntg_xy_is_greater_eq(dest_start_pos, dest_end_pos))
        ntg_vreturn(out_status, NTG_ERR_OUT_OF_BOUNDS);
    if(ntg_xy_is_greater_eq(dest_end_pos, dest_size))
        ntg_vreturn(out_status, NTG_ERR_OUT_OF_BOUNDS);

    size_t i, j;
    struct ntg_xy it_dest_pos, it_src_pos;
    struct ntg_vcell it_src_cell;
    for(i = 0; i < src_size.y; i++)
    {
        for(j = 0; j < src_size.x; j++)
        {
            it_dest_pos = ntg_xy_add(dest_start_pos, ntg_xy(j, i));
            it_src_pos = ntg_xy(j, i);

            it_src_cell = ntg_object_drawing_get(src_drawing, it_src_pos);
            ntg_object_drawing_set(dest_drawing, it_src_cell, it_dest_pos);
        }
    }
}

void ntg_object_drawing_place_(
        const ntg_object_drawing* src_drawing,
        ntg_stage_drawing* dest_drawing,
        struct ntg_xy dest_start_pos,
        int* out_status)
{
    ntg_init_status(out_status);

    if(!src_drawing || !dest_drawing)
    {
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);
    }

    struct ntg_xy dest_size = ntg_stage_drawing_get_size(dest_drawing);
    struct ntg_xy src_size = ntg_object_drawing_get_size(src_drawing);

    if(ntg_xy_size_is_zero(src_size)) return; // success, nothing to draw

    struct ntg_xy dest_end_pos = ntg_xy_add(dest_start_pos, src_size);

    if(ntg_xy_is_greater_eq(dest_start_pos, dest_end_pos))
        ntg_vreturn(out_status, NTG_ERR_OUT_OF_BOUNDS);
    if(ntg_xy_is_greater_eq(dest_end_pos, dest_size))
        ntg_vreturn(out_status, NTG_ERR_OUT_OF_BOUNDS);

    size_t i, j;
    struct ntg_xy it_dest_pos, it_src_pos;
    struct ntg_vcell it_src_cell;
    struct ntg_cell it_base_cell;
    struct ntg_cell it_overwritten;
    for(i = 0; i < src_size.y; i++)
    {
        for(j = 0; j < src_size.x; j++)
        {
            it_dest_pos = ntg_xy_add(dest_start_pos, ntg_xy(j, i));
            it_src_pos = ntg_xy(j, i);

            it_src_cell = ntg_object_drawing_get(src_drawing, it_src_pos);
            it_base_cell = ntg_stage_drawing_get(dest_drawing, it_dest_pos);
            it_overwritten = ntg_vcell_overwrite(it_src_cell, it_base_cell);
            ntg_stage_drawing_set(dest_drawing, it_overwritten, it_dest_pos);
        }
    }
}
