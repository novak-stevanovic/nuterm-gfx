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

int ntg_stage_draw_set_size(ntg_stage_draw* drawing, struct ntg_xy size)
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

    drawing->ro.size = size;

    if(new_size_prod > curr_size_prod)
    {
        size_t diff = new_size_prod - curr_size_prod;

        for(i = 0; i < diff; i++)
        {
            status = ntg_cell_vec_pushb(cell_vec, ntg_cell_default());
            if(status)
            {
                ntg_cell_vec_popb_many(cell_vec, i);
                return NTG_ERR_ALLOC_FAIL;
            }
        }

        for(i = 0; i < curr_size_prod; i++)
            cell_vec->data[i] = ntg_cell_default();

        return 0;
    }
    else
    {
        size_t diff = curr_size_prod - new_size_prod;

        ntg_cell_vec_popb_many_shrink(cell_vec, diff);

        for(i = 0; i < new_size_prod; i++)
            cell_vec->data[i] = ntg_cell_default();

        return 0;
    }
}
