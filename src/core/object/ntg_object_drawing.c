#include "ntg.h"
#include <stdlib.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_object_drawing_init(ntg_object_drawing* drawing)
{
    if(!drawing) return NTG_ERR_INV_ARG;

    if(ntg_vcell_vecgrid_init(&drawing->priv.data))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

int ntg_object_drawing_deinit(ntg_object_drawing* drawing)
{
    if(!drawing) return NTG_ERR_INV_ARG;

    if(ntg_vcell_vecgrid_deinit(&drawing->priv.data))
        return NTG_ERR_UNEXPECTED;

    return 0;
}

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing* drawing)
{
    return (drawing != NULL) ?
        ntg_vcell_vecgrid_get_size(&drawing->priv.data) :
        NTG_XY_UNSET;
}

int ntg_object_drawing_set_size(
        ntg_object_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap)
{
    if(!drawing)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX) ||
    (size_cap.x > NTG_SIZE_MAX) || (size_cap.y > NTG_SIZE_MAX))
    {
        return NTG_ERR_INV_ARG;
    }

    return ntg_vcell_vecgrid_set_size(&drawing->priv.data, size, size_cap);
}

/* ------------------------------------------------------ */
/* PLACEMENT */
/* ------------------------------------------------------ */

int ntg_object_drawing_place(
        const ntg_object_drawing* src_drawing,
        ntg_object_drawing* dest_drawing,
        struct ntg_xy dest_start_pos)
{
    if(!src_drawing || !dest_drawing || (src_drawing == dest_drawing))
    {
        return NTG_ERR_INV_ARG;
    }

    struct ntg_xy dest_size = ntg_object_drawing_get_size(dest_drawing);
    struct ntg_xy src_size = ntg_object_drawing_get_size(src_drawing);

    if(ntg_xy_is_zero_any(src_size)) return 0; 

    struct ntg_xy dest_end_pos = ntg_xy_add(dest_start_pos, src_size);

    if(!ntg_xy_is_lesser(dest_start_pos, dest_end_pos))
        return NTG_ERR_OUT_OF_BOUNDS;
    if(!ntg_xy_is_lesser_eq(dest_size, dest_end_pos))
        return NTG_ERR_OUT_OF_BOUNDS;

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

    return 0;
}

int ntg_object_drawing_place_(
        const ntg_object_drawing* src_drawing,
        ntg_stage_drawing* dest_drawing,
        struct ntg_xy dest_start_pos)
{
    if(!src_drawing || !dest_drawing)
    {
        return NTG_ERR_INV_ARG;
    }

    struct ntg_xy dest_size = ntg_stage_drawing_get_size(dest_drawing);
    struct ntg_xy src_size = ntg_object_drawing_get_size(src_drawing);

    if(ntg_xy_is_zero_any(src_size)) return 0; 

    struct ntg_xy dest_end_pos = ntg_xy_add(dest_start_pos, src_size);

    if(!ntg_xy_is_lesser(dest_start_pos, dest_end_pos))
        return NTG_ERR_OUT_OF_BOUNDS;
    if(!ntg_xy_is_lesser_eq(dest_end_pos, dest_size))
        return NTG_ERR_OUT_OF_BOUNDS;

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
    return 0;
}
