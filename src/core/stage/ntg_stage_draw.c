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

int ntg_stage_draw_init(ntg_stage_draw* drawing)
{
    if(!drawing) return NTG_ERR_INV_ARG;

    (*drawing) = (ntg_stage_draw) {0};

    return 0;
}

int ntg_stage_draw_deinit(ntg_stage_draw* drawing)
{
    if(!drawing) return NTG_ERR_INV_ARG;

    ntg_cell_vec_deinit(&drawing->priv.cell_vec);

    (*drawing) = (ntg_stage_draw) {0};

    return 0;
}

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

int ntg_stage_draw_set_size(ntg_stage_draw* drawing, ntg_xy size)
{
    if(!drawing)
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    int status;

    size_t new_size_prod = size.x * size.y; 
    size_t curr_size_prod = drawing->ro.size.x * drawing->ro.size.y;
    struct ntg_cell_vec* cell_vec = &(drawing->priv.cell_vec);

    size_t i;

    if(new_size_prod > curr_size_prod)
    {
        size_t diff = new_size_prod - curr_size_prod;

        for(i = 0; i < diff; i++)
        {
            status = ntg_cell_vec_pushb(cell_vec, NTG_CELL_ZERO);
            if(status)
            {
                ntg_cell_vec_popb_many(cell_vec, i);
                return NTG_ERR_ALLOC_FAIL;
            }
        }

        memset(cell_vec->data, 0, curr_size_prod * sizeof(struct ntg_cell));
    }
    else
    {
        size_t diff = curr_size_prod - new_size_prod;

        ntg_cell_vec_popb_many_shrink(cell_vec, diff);

        if(new_size_prod)
            memset(cell_vec->data, 0, new_size_prod * sizeof(struct ntg_cell));
    }

    drawing->ro.size = size;
    return 0;
}
