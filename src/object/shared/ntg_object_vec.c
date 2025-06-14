#include "object/shared/ntg_object_vec.h"
#include <assert.h>

#define _NTG_VECTOR_IMPLEMENTATION_
#include "shared/ntg_vector.h"
#undef _NTG_VECTOR_IMPLEMENTATION_

void __ntg_object_vec_init__(ntg_object_vec_t* vec)
{
    int status = __ntg_vector_init__((struct ntg_vector*)vec,
            0, sizeof(ntg_object_t*));

    assert(status == 0);
}

void __ntg_object_vec_deinit__(ntg_object_vec_t* vec)
{
    __ntg_vector_deinit__((struct ntg_vector*)vec);
}

ntg_object_vec_t* ntg_object_vec_new()
{
    ntg_object_vec_t* new = (ntg_object_vec_t*)malloc(
            sizeof(struct ntg_object_vec));

    if(new == NULL) return NULL;

    __ntg_object_vec_init__(new);
    
    return new;
}

void ntg_object_vec_destroy(ntg_object_vec_t* vec)
{
    if(vec == NULL) return;

    __ntg_object_vec_deinit__(vec);

    free(vec);
}

void ntg_object_vec_append(ntg_object_vec_t* vec, ntg_object_t* object)
{
    int status = ntg_vector_append((struct ntg_vector*)vec,
            &object, sizeof(ntg_object_t*));

    assert(status == 0);
}

void ntg_object_vec_remove(ntg_object_vec_t* vec, ntg_object_t* object)
{
    ntg_vector_remove((struct ntg_vector*)vec, &object,
            sizeof(ntg_object_t*), NULL);
}
