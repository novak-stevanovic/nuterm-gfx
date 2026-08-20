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

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

int ntg_vecgrid_init(ntg_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    vecgrid->ro.data = NULL;
    vecgrid->ro.size = ntg_xy(0, 0);
    vecgrid->ro.capacity = 0;

    return 0;
}

int ntg_vecgrid_deinit(ntg_vecgrid* vecgrid)
{
    if(!vecgrid) return NTG_ERR_INV_ARG;

    if(vecgrid->ro.data != NULL)
        free(vecgrid->ro.data);

    vecgrid->ro.data = NULL;
    vecgrid->ro.size = ntg_xy(0, 0);
    vecgrid->ro.capacity = 0;

    return 0;
}

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

int ntg_vecgrid_set_size(
        ntg_vecgrid* vecgrid,
        struct ntg_xy size,
        double modifier,
        struct ntg_xy size_cap,
        size_t data_size)
{
    if((!vecgrid) || (data_size == 0))
        return NTG_ERR_INV_ARG;

    if((size.x > NTG_SIZE_MAX) || (size.y > NTG_SIZE_MAX) ||
    (size_cap.x > NTG_SIZE_MAX) || (size_cap.y > NTG_SIZE_MAX))
    {
        return NTG_ERR_INV_ARG;
    }

    if(modifier < 1.1) modifier = 1.1;
    size_cap.x = _max2_size(size_cap.x, 1);
    size_cap.y = _max2_size(size_cap.y, 1);

    size = ntg_xy_size(size);

    if(ntg_xy_are_eql(vecgrid->ro.size, size)) return 0;

    size_t size_prod = size.x * size.y;
    
    size_t size_cap_prod = size_cap.x * size_cap.y;
    size_t shrink_threshold = vecgrid->ro.capacity / modifier;

    if((size_prod > vecgrid->ro.capacity) || (size_prod <= shrink_threshold))
    {
        size_t old_cap = vecgrid->ro.capacity;
        size_t new_cap;

        if(size_prod == 0)
        {
            new_cap = 0;
            free(vecgrid->ro.data);

            vecgrid->ro.data = NULL;
            vecgrid->ro.capacity = new_cap;
        }
        else
        {
            double grown_cap = (double)size_prod * modifier;
            if(grown_cap >= (double)size_cap_prod)
                new_cap = size_cap_prod;
            else
                new_cap = (size_t)grown_cap;

            new_cap = _max2_size(new_cap, size_prod);

            if(new_cap > SIZE_MAX / data_size)
                return NTG_ERR_OUT_OF_BOUNDS;

            void* new_data;
            if(old_cap > 0)
                new_data = realloc(vecgrid->ro.data, new_cap * data_size);
            else
                new_data = malloc(new_cap * data_size);

            if(!new_data)
                return NTG_ERR_ALLOC_FAIL;

            vecgrid->ro.data = new_data;
            vecgrid->ro.capacity = new_cap;
        }
    }

    vecgrid->ro.size = size;

    return 0;
}
