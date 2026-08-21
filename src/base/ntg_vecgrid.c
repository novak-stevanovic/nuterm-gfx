#include "base/ntg_vecgrid.h"
#include "shared/ntg_log.h"
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "shared/ntg_shared_internal.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_vecgrid_init(struct ntg_vecgrid* vecgrid, double cap_factor)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(cap_factor < NTG_VECGRID_CAP_FACTOR_MIN)
        return NTG_ERR_INV_ARG;

    (*vecgrid) = (struct ntg_vecgrid) {0};
    vecgrid->cap_factor = cap_factor;

    return 0;
}

int ntg_vecgrid_deinit(struct ntg_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(vecgrid->data != NULL)
        free(vecgrid->data);

    (*vecgrid) = (struct ntg_vecgrid) {0};

    return 0;
}

int ntg_vecgrid_set_size(struct ntg_vecgrid* vecgrid, struct ntg_xy size, size_t data_size)
{
    if((!vecgrid) || (data_size == 0))
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX))
        return NTG_ERR_INV_ARG;

    size = ntg_xy_size(size);
    size_t size_prod = size.x * size.y;

    if(ntg_xy_are_eql(vecgrid->size, size)) return 0;

    if(size_prod > vecgrid->cap) // grow
    {
        /* new_cap must be greater than size_prod */
        size_t new_cap = size_prod * vecgrid->cap_factor;

        void* new_data = realloc(vecgrid->data, new_cap * data_size);
        if(!new_data)
            return NTG_ERR_ALLOC_FAIL;

        vecgrid->cap = new_cap;
        vecgrid->size = size;
        vecgrid->data = new_data;
    }
    else // don't grow
    {
        size_t low_thresh = vecgrid->cap / vecgrid->cap_factor / vecgrid->cap_factor;
        if(low_thresh == 0) low_thresh = 1;
    
        if(size_prod < low_thresh) // shrink
        {
            if(size_prod == 0) // free
            {
                if(vecgrid->data) free(vecgrid->data);

                vecgrid->data = NULL;
                vecgrid->cap = 0;
                vecgrid->size = ntg_xy(0, 0);

                return 0;
            }

            size_t new_cap = size_prod * vecgrid->cap_factor;

            void* new_data = realloc(vecgrid->data, new_cap * data_size);
            if(!new_data) return NTG_ERR_ALLOC_FAIL;

            vecgrid->cap = new_cap;
            vecgrid->size = size;
            vecgrid->data = new_data;
        }
        else
        {
            vecgrid->size = size;
        }
    }

    return 0;
}
