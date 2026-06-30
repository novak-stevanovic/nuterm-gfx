#include "base/ntg_vecgrid.h"
#include "shared/ntg_log.h"
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "shared/ntg_shared_internal.h"

void ntg_vecgrid_init(ntg_vecgrid* vecgrid)
{
    if(!vecgrid) return;

    vecgrid->_data = NULL;
    vecgrid->_size = ntg_xy(0, 0);
    vecgrid->_capacity = 0;
}

void ntg_vecgrid_deinit(ntg_vecgrid* vecgrid)
{
    if(!vecgrid) return;

    if(vecgrid->_data != NULL)
        free(vecgrid->_data);

    vecgrid->_data = NULL;
    vecgrid->_size = ntg_xy(0, 0);
    vecgrid->_capacity = 0;
}

void ntg_vecgrid_set_size(
        ntg_vecgrid* vecgrid,
        struct ntg_xy size,
        double modifier,
        struct ntg_xy size_cap,
        size_t data_size,
        int* out_status)
{
    ntg_init_status(out_status);

    if((!vecgrid) || (data_size == 0) || (modifier <= 1.05))
        ntg_vreturn(out_status, NTG_ERR_INVALID_ARG);

    size_cap.x = _max2_size(size_cap.x, 1);
    size_cap.y = _max2_size(size_cap.y, 1);

    size = ntg_xy_size(size);

    if(ntg_xy_are_equal(vecgrid->_size, size)) return;

    if((size.x != 0) && (size.y > SIZE_MAX / size.x))
        ntg_vreturn(out_status, NTG_ERR_OUT_OF_BOUNDS);

    if((size_cap.x != 0) && (size_cap.y > SIZE_MAX / size_cap.x))
        ntg_vreturn(out_status, NTG_ERR_OUT_OF_BOUNDS);

    size_t size_prod = size.x * size.y;
    // struct ntg_xy old_size = vecgrid->_size;
    // size_t old_size_prod = old_size.x * old_size.y;
    size_t size_cap_prod = size_cap.x * size_cap.y;
    size_t shrink_threshold = vecgrid->_capacity / modifier;

    if((size_prod > vecgrid->_capacity) || (size_prod <= shrink_threshold))
    {
        size_t old_cap = vecgrid->_capacity;
        size_t new_cap;

        if(size_prod == 0)
        {
            new_cap = 0;
            free(vecgrid->_data);

            vecgrid->_data = NULL;
            vecgrid->_capacity = new_cap;
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
                ntg_vreturn(out_status, NTG_ERR_OUT_OF_BOUNDS);

            void* new_data;
            if(old_cap > 0)
                new_data = realloc(vecgrid->_data, new_cap * data_size);
            else
                new_data = malloc(new_cap * data_size);

            if(!new_data)
                ntg_vreturn(out_status, NTG_ERR_ALLOC_FAIL);

            vecgrid->_data = new_data;
            vecgrid->_capacity = new_cap;
        }
    }

    vecgrid->_size = size;
}
