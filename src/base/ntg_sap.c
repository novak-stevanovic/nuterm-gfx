#include "base/ntg_sap.h"
#include "shared/_ntg_shared.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// TODO: fix & test
size_t ntg_sap_cap_round_robin(const size_t* caps, const size_t* grows,
        size_t* out_sizes, size_t space_pool, size_t count)
{
    assert(caps != NULL);
    assert(out_sizes != NULL);
    
    if((space_pool == 0) || (count == 0)) return 0;

    size_t total_grow = 0;

    double _space_pool = space_pool;

    double* distributed = (double*)malloc(sizeof(double) * count);
    assert(distributed != NULL);

    size_t i;

    for(i = 0; i < count; i++)
    {
        if(grows != NULL)
            total_grow += grows[i];
        distributed[i] = 0;
    }

    bool loop = true;
    double it_amount;
    while(loop)
    {
        loop = false;
        for(i = 0; i < count; i++)
        {
            if(caps[i] > (out_sizes[i] + distributed[i]))
            {
                // TODO: what if not max sizes?
                if(grows != NULL)
                {
                    it_amount = _min2_double(_space_pool, (total_grow != 0) ?
                            (1.0 * grows[i] / total_grow) : 0);
                }
                else
                    it_amount = _min2_double(_space_pool, 1);


                if(it_amount < 0.0005) continue;

                if((1.0 * caps[i]) >= ((1.0 * out_sizes[i]) +
                            distributed[i] + it_amount))
                {
                    distributed[i] += it_amount;
                    _space_pool -= it_amount;

                    if(_space_pool <= 0.0005)
                    {
                        loop = false;
                        break;
                    }
                    else loop = true;
                }
            }
        }
    }

    size_t distributed_total = 0;

    for(i = 0; i < count; i++)
    {
        out_sizes[i] += distributed[i];
        distributed_total += floor(distributed[i]);
    }

    space_pool -= distributed_total;
    for(i = 0; i < count; i++)
    {
        if(space_pool == 0) break;

        if(caps[i] > (out_sizes[i] + floor(distributed[i])))
        {
            (out_sizes[i])++;
            distributed_total++;
            space_pool--;
        }
    }

    free(distributed);

    return distributed_total;
}
