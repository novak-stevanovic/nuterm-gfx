#include "ntg.h"

GENC_VECTOR_DEFINE(ntg_widget_vec, ntg_widget*, 1.5, )

size_t ntg_widget_vec_find(const struct ntg_widget_vec* vec, ntg_widget* widget)
{
    if(!vec || !widget) return GENC_ERR_INV_ARG;
    
    size_t i;
    for(i = 0; i < vec->size; i++)
    {
        if(vec->data[i] == widget)
            return i;
    }

    return SIZE_MAX;
}

bool ntg_widget_vec_exists(const struct ntg_widget_vec* vec, ntg_widget* widget)
{
    if(!vec || !widget) return GENC_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < vec->size; i++)
    {
        if(vec->data[i] == widget)
            return true;
    }

    return false;
}

int ntg_widget_vec_rm(struct ntg_widget_vec* vec, const ntg_widget* widget)
{
    if(!vec || !widget) return GENC_ERR_INV_ARG;

    size_t i;
    for(i = 0; i < vec->size; i++)
    {
        if(vec->data[i] == widget)
            return ntg_widget_vec_rm_at(vec, i);
    }

    return GENC_ERR_NO_DATA;
}
