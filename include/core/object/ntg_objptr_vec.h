#ifndef NTG_OBJPTR_VEC_H
#define NTG_OBJPTR_VEC_H

#include "shared/ntg_shared.h"
#include "thirdparty/genc.h"

GENC_VECTOR_GENERATE(ntg_objptr_vec, ntg_object*, 1.5)

static inline int
ntg_objptr_vec_init(ntg_objptr_vec* vec, size_t cap)
{
    if(!vec)
        return GENC_ERR_INVALID_ARG;

    (*vec) = (ntg_objptr_vec) {0};
    return ntg_objptr_vec_prealloc(vec, cap);
}

static inline int
ntg_objptr_vec_rm(ntg_objptr_vec* vec, const ntg_object* object)
{
    if(!vec)
        return GENC_ERR_INVALID_ARG;

    ntg_object** data = ntg_objptr_vec_data(vec);
    size_t size = ntg_objptr_vec_size(vec);

    size_t i;
    for(i = 0; i < size; i++)
    {
        if(data[i] == object)
            return ntg_objptr_vec_rm_at(vec, i);
    }

    return GENC_ERR_NO_DATA;
}

#endif // NTG_OBJPTR_VEC_H
