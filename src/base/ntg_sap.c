#include "base/ntg_sap.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

static inline bool __is_equal_double(double x, double y)
{
    return fabs(x - y) < 0.005;
}

// TODO: rethink
size_t ntg_sap_cap_round_robin(const size_t* caps, const size_t* grows,
        size_t* out_sizes, size_t space_pool, size_t count)
{
    assert(caps != NULL);
    assert(out_sizes != NULL);
    
    if((space_pool == 0) || (count == 0)) return 0;

    size_t i;
    size_t total_grow = 0;
    double _space_pool = (double)space_pool;
    double distributed_total = 0;
    double space_left = space_pool;
    size_t it_grow;

    /* Compute total grow */
    for(i = 0; i < count; i++)
    {
        it_grow = (grows != NULL) ? grows[i] : 1;
        total_grow += it_grow;
    }

    if(total_grow == 0) return 0;

    double* distributed = (double*)malloc(sizeof(double) * count);
    for(i = 0; i < count; i++) distributed[i] = 0;

    double it_grow_factor;
    double it_to_distribute;
    double it_distribute_cap;
    
    /* Initial distribution */
    bool loop = true;
    while(loop)
    {
        loop = false;
        for(i = 0; i < count; i++)
        {
            it_grow = (grows != NULL) ? grows[i] : 1;
            it_grow_factor = (1.0 * it_grow) / total_grow;
            if(it_grow_factor == 0) continue;

            /* Test space_left */
            if(__is_equal_double(space_left, 0)) break;
            /* Test caps[i] */
            if(out_sizes[i] + distributed[i] >= caps[i]) continue;

            it_to_distribute = _min2_double(space_left, it_grow_factor);
            it_to_distribute = _min2_double(
                    it_to_distribute,
                    caps[i] - out_sizes[i] - distributed[i]);
            if(__is_equal_double(it_to_distribute, 0)) continue;

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
        assert(out_sizes[i] <= caps[i]);
        if(out_sizes[i] == caps[i]) continue;
        if(space_pool == 0) break;

        out_sizes[i]++;
        distributed_actual++;
        space_pool--;
    }

    free(distributed);

    return distributed_actual;
}
