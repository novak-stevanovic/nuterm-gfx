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

int ntg_object_draw_init(ntg_object_draw* drawing)
{
    if(!drawing) return NTG_ERR_INV_ARG;

    (*drawing) = (ntg_object_draw) {0};

    return 0;
}

int ntg_object_draw_deinit(ntg_object_draw* drawing)
{
    if(!drawing) return NTG_ERR_INV_ARG;

    ntg_vcell_vec_deinit(&drawing->priv.vcell_vec);

    (*drawing) = (ntg_object_draw) {0};

    return 0;
}

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

int ntg_object_draw_set_size(ntg_object_draw* drawing, struct ntg_xy size)
{
    if(!drawing)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    int status;

    size_t new_size_prod = size.x * size.y; 
    size_t curr_size_prod = drawing->ro.size.x * drawing->ro.size.y;
    struct ntg_vcell_vec* vcell_vec = &(drawing->priv.vcell_vec);

    size_t i;

    if(new_size_prod > curr_size_prod)
    {
        size_t diff = new_size_prod - curr_size_prod;

        for(i = 0; i < diff; i++)
        {
            status = ntg_vcell_vec_pushb(vcell_vec, NTG_VCELL_ZERO);
            if(status)
            {
                ntg_vcell_vec_popb_many(vcell_vec, i);
                return NTG_ERR_ALLOC_FAIL;
            }
        }

        memset(vcell_vec->data, 0, curr_size_prod * sizeof(struct ntg_vcell));
    }
    else
    {
        size_t diff = curr_size_prod - new_size_prod;

        ntg_vcell_vec_popb_many_shrink(vcell_vec, diff);

        if(new_size_prod)
            memset(vcell_vec->data, 0, new_size_prod * sizeof(struct ntg_vcell));
    }

    drawing->ro.size = size;
    return 0;
}

/* ------------------------------------------------------ */
/* PLACEMENT */
/* ------------------------------------------------------ */

int ntg_object_draw_place(
        const ntg_object_draw* src_drawing,
        ntg_object_draw* dest_drawing,
        struct ntg_xy dest_start_pos)
{
    if(!src_drawing || !dest_drawing || (src_drawing == dest_drawing))
        return NTG_ERR_INV_ARG;

    struct ntg_xy dest_size = dest_drawing->ro.size;
    struct ntg_xy src_size = src_drawing->ro.size;

    if(ntg_xy_is_zero_any(src_size)) return 0; 

    struct ntg_xy dest_end_pos = ntg_xy_add(dest_start_pos, src_size);

    if(!ntg_xy_is_lesser(dest_start_pos, dest_end_pos))
        return NTG_ERR_OUT_OF_BOUNDS;
    if(!ntg_xy_is_lesser_eq(dest_end_pos, dest_size))
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

            it_src_cell = ntg_object_draw_get(src_drawing, it_src_pos);
            ntg_object_draw_set(dest_drawing, it_src_cell, it_dest_pos);
        }
    }

    return 0;
}

/* ------------------------------------------------------ */

int ntg_object_draw_place_(
        const ntg_object_draw* src_drawing,
        ntg_stage_draw* dest_drawing,
        struct ntg_xy dest_start_pos)
{
    if(!src_drawing || !dest_drawing)
    {
        return NTG_ERR_INV_ARG;
    }

    struct ntg_xy dest_size = dest_drawing->ro.size;
    struct ntg_xy src_size = src_drawing->ro.size;

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

            it_src_cell = ntg_object_draw_get(src_drawing, it_src_pos);
            it_base_cell = ntg_stage_draw_get(dest_drawing, it_dest_pos);
            it_overwritten = ntg_vcell_overwrite(it_src_cell, it_base_cell);
            ntg_stage_draw_set(dest_drawing, it_overwritten, it_dest_pos);
        }
    }
    return 0;
}
