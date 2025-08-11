#include "base/ntg_sap.h"
#include <assert.h>
#include <stdbool.h>

size_t ntg_sap_cap_round_robin(const size_t* caps, size_t* out_sizes,
        size_t space_pool, size_t count)
{
    assert(out_sizes != NULL);

    size_t i;
    size_t distributed = 0;

    bool loop = (space_pool > 0);
    while(loop)
    {
        loop = false;
        for(i = 0; i < count; i++)
        {
            if(caps[i] > out_sizes[i])
            {
                (out_sizes[i])++;
                space_pool--;
                distributed++;

                loop = (space_pool > 0);
                if(!loop) break;
            }
        }
    }

    return distributed;
}
