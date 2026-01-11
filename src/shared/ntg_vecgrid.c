#include "shared/ntg_vecgrid.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_log.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

void ntg_vecgrid_init(ntg_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    vecgrid->_data = NULL;
    vecgrid->_size = ntg_xy(0, 0);
    vecgrid->_capacity = 0;
}

void ntg_vecgrid_deinit(ntg_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    if(vecgrid->_data != NULL)
        free(vecgrid->_data);

    vecgrid->_data = NULL;
    vecgrid->_size = ntg_xy(0, 0);
    vecgrid->_capacity = 0;
}

static size_t counter = 0;

void ntg_vecgrid_set_size(ntg_vecgrid* vecgrid, struct ntg_xy size,
    double modifier, struct ntg_xy size_cap, size_t data_size)
{
    assert(vecgrid != NULL);
    assert(data_size > 0);
    assert(modifier > 1.0);

    size = ntg_xy_size(size);

    if(ntg_xy_are_equal(vecgrid->_size, size)) return;

    size_t size_prod = size.x * size.y;
    size_t old_size_prod = vecgrid->_size.x * vecgrid->_size.y;
    size_t size_cap_prod = size_cap.x * size_cap.y;

    size_t shrink_threshold = vecgrid->_capacity / modifier;

    /*
    ntg_log_log("VECGRID SET SIZE | %p | OLD SIZE: %d %d - %d | CAPACITY: %d | NEW SIZE: %d %d - %d | SIZE_CAP: %d %d - %d | SHRINK_THRESHOLD: %d",
        vecgrid,
        vecgrid->_size.x, vecgrid->_size.y, old_size_prod,
        vecgrid->_capacity,
        size.x, size.y, size_prod,
        size_cap.x, size_cap.y, size_cap_prod,
        shrink_threshold);
    */

    if((size_prod > vecgrid->_capacity) || (size_prod <= shrink_threshold))
    {
        if(size_prod == 0)
        {
            // ntg_log_log("VECGRID SET SIZE | FREEING");
            if(vecgrid->_capacity > 0)
                free(vecgrid->_data);

            vecgrid->_data = 0;
            vecgrid->_capacity = 0;
        }
        else
        {
            size_t new_cap = _min2_size(size_prod * modifier, size_cap_prod);
            // ntg_log_log("REALLOC %d", ++counter);
            if(vecgrid->_capacity > 0)
                vecgrid->_data = realloc(vecgrid->_data, new_cap * data_size);
            else
                vecgrid->_data = malloc(new_cap * data_size);
            assert(vecgrid->_data != NULL);
            vecgrid->_capacity = new_cap;
        }
    }

    // if(size_prod == 0)
    // {
    //     // ntg_log_log("VECGRID SET SIZE | FREEING");
    //     if(vecgrid->_capacity > 0)
    //         free(vecgrid->_data);

    //     vecgrid->_data = 0;
    //     vecgrid->_capacity = 0;
    // }
    // else
    // {
    //     if(vecgrid->_capacity > 0)
    //         vecgrid->_data = realloc(vecgrid->_data, size_prod * data_size);
    //     else
    //         vecgrid->_data = malloc(size_prod * data_size);
    // }


    vecgrid->_size = size;
}
