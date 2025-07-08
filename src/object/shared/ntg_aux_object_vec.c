#include "object/shared/ntg_aux_object_vec.h"
#include <assert.h>

#define _NTG_VECTOR_IMPLEMENTATION_
#include "shared/ntg_vector.h"
#undef _NTG_VECTOR_IMPLEMENTATION_

void __ntg_aux_object_vec_init__(ntg_aux_object_vec* vec)
{
    int status = __ntg_vector_init__((struct ntg_vector*)vec,
            0, sizeof(struct ntg_aux_object));

    assert(status == 0);
}

void __ntg_aux_object_vec_deinit__(ntg_aux_object_vec* vec)
{
    __ntg_vector_deinit__((struct ntg_vector*)vec);
}

ntg_aux_object_vec* ntg_aux_object_vec_new()
{
    ntg_aux_object_vec* new = (ntg_aux_object_vec*)malloc(
            sizeof(struct ntg_aux_object_vec));

    if(new == NULL) return NULL;

    __ntg_aux_object_vec_init__(new);
    
    return new;
}

void ntg_aux_object_vec_destroy(ntg_aux_object_vec* vec)
{
    if(vec == NULL) return;

    __ntg_aux_object_vec_deinit__(vec);

    free(vec);
}

void ntg_aux_object_vec_insert(ntg_aux_object_vec* vec,
        struct ntg_aux_object aux_object)
{
    ssize_t i = -1;
    for(i = 0; i < vec->_count; i++)
    {
        if(vec->_data[i].z_index >= aux_object.z_index)
            break;
    }

    if(i == -1)
    {
        ntg_vector_append((struct ntg_vector*)vec, &aux_object,
                sizeof(struct ntg_aux_object));
    }
    else
    {
        ntg_vector_insert((struct ntg_vector*)vec,
                &aux_object,
                i,
                sizeof(struct ntg_aux_object));
    }
}

static bool __cmp_fn(const void* _el_ptr, const void* _data)
{
    const struct ntg_aux_object* el_ptr = (const struct ntg_aux_object*)_el_ptr;
    const ntg_object* data = (const ntg_object*)_data;

    return (el_ptr->object == data);
}

void ntg_aux_object_vec_remove(ntg_aux_object_vec* vec, ntg_object* object)
{
    ntg_vector_remove((struct ntg_vector*)vec, &object,
            sizeof(struct ntg_aux_object), __cmp_fn);
}

void ntg_aux_object_vec_empty(ntg_aux_object_vec* vec)
{
    while(vec->_count > 0)
    {
        ntg_aux_object_vec_remove(vec, vec->_data[0].object);
    }
}
