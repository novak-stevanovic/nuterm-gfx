#include "ntg.h"

GENC_VECTOR_DEFINE(ntg_objptr_vec, ntg_object*, 1.5, )

size_t ntg_objptr_vec_find(const struct ntg_objptr_vec* vec, ntg_object* object)
{
    if(!vec || !object) return GENC_ERR_INV_ARG;
    
    size_t i;
    for(i = 0; i < vec->size; i++)
    {
        if(vec->data[i] == object)
            return i;
    }

    return SIZE_MAX;
}

bool ntg_objptr_vec_exists(const struct ntg_objptr_vec* vec, ntg_object* object)
{
    if(!vec || !object) return GENC_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < vec->size; i++)
    {
        if(vec->data[i] == object)
            return true;
    }

    return false;
}

int ntg_objptr_vec_rm(struct ntg_objptr_vec* vec, const ntg_object* object)
{
    if(!vec || !object) return GENC_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < vec->size; i++)
    {
        if(vec->data[i] == object)
            return ntg_objptr_vec_rm_at(vec, i);
    }

    return GENC_ERR_NO_DATA;
}
