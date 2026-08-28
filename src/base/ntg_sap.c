#include "ntg.h"

#include "shared/ntg_shared_internal.h"
#include <math.h>

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* STATIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

static inline bool is_equal_double(double x, double y)
{
    return fabs(x - y) < 0.005;
}

// TODO: optimize

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

int ntg_sap_cap_round_robin(
        const size_t* caps,
        const size_t* grows,
        size_t space_pool,
        size_t count,
        double* scratch_buffer,
        size_t* out_sizes,
        size_t* out_distributed)
{
    if(!caps || !out_sizes || ((count > 0) && !scratch_buffer))
        return NTG_ERR_INV_ARG;

    ntg_set_out(out_distributed, 0);
    
    if((space_pool == 0) || (count == 0)) return 0;

    size_t i;
    size_t total_grow = 0;
    double _space_pool = (double)space_pool;
    double distributed_total = 0;
    double space_left = space_pool;
    size_t it_grow;

    
    for(i = 0; i < count; i++)
    {
        it_grow = (grows != NULL) ? grows[i] : 1;
        total_grow += it_grow;
    }

    if(total_grow == 0) return 0;

    double* distributed = scratch_buffer;
    for(i = 0; i < count; i++)
        distributed[i] = 0;

    double it_grow_factor;
    double it_to_distribute;
    
    
    bool loop = true;
    while(loop)
    {
        loop = false;
        for(i = 0; i < count; i++)
        {
            it_grow = (grows != NULL) ? grows[i] : 1;
            it_grow_factor = (1.0 * it_grow) / total_grow;
            if(it_grow_factor == 0) continue;

            
            if(is_equal_double(space_left, 0)) break;
            
            if(out_sizes[i] + distributed[i] >= caps[i]) continue;

            it_to_distribute = ntg_min2_double(space_left, it_grow_factor);
            it_to_distribute = ntg_min2_double(
                    it_to_distribute,
                    caps[i] - out_sizes[i] - distributed[i]);
            if(is_equal_double(it_to_distribute, 0)) continue;

            distributed[i] += it_to_distribute;
            distributed_total += it_to_distribute;
            space_left = _space_pool - distributed_total;

            loop = true;
        }
    }

    size_t distributed_actual = 0;
    size_t it_floored;

    for(i = 0; i < count; i++)
    {
        it_floored = floor(distributed[i]);
        distributed_actual += (size_t)it_floored;
        out_sizes[i] += (size_t)it_floored;
    }

    space_pool -= distributed_actual;

    for(i = 0; i < count; i++)
    {
        it_grow = (grows != NULL) ? grows[i] : 1;

        if(it_grow == 0) continue;
        if(out_sizes[i] > caps[i])
            return NTG_ERR_UNEXPECTED;
        if(out_sizes[i] == caps[i]) continue;
        if(space_pool == 0) break;

        out_sizes[i]++;
        distributed_actual++;
        space_pool--;
    }

    ntg_set_out(out_distributed, distributed_actual);
    return 0;
}
