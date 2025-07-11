#include "base/ntg_sap.h"
#include <assert.h>
#include <stdbool.h>

void ntg_sap_nsize_round_robin(const size_t* nsizes, size_t* out_sizes,
        size_t space_pool, size_t count)
{
    if((nsizes == NULL) || (out_sizes == NULL)) assert(0);

    size_t i;
    for(i = 0; i < count; i++)
        out_sizes[i] = 0;

    bool loop = (space_pool > 0);
    while(loop)
    {
        loop = false;
        for(i = 0; i < count; i++)
        {
            if(nsizes[i] > out_sizes[i])
            {
                (out_sizes[i])++;
                space_pool--;

                loop = (space_pool > 0);
                if(!loop) break;
            }
        }
    }
}

// void ntg_sap_nsize_round_robin_sub(const size_t* nsizes, size_t* curr_sizes,
//         size_t space, size_t count)
// {
//     if((nsizes == NULL) || (curr_sizes == NULL)) assert(0);
//
//     bool loop = (space_pool > 0);
//     while(loop)
//     {
//         loop = false;
//         for(i = 0; i < count; i++)
//         {
//             if(nsizes[i] > out_sizes[i])
//             {
//                 (out_sizes[i])++;
//                 space_pool--;
//
//                 loop = (space_pool > 0);
//                 if(!loop) break;
//             }
//         }
//     }
// }
