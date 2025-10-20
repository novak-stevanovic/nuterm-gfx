#include "core/object/shared/ntg_object_vec.h"
#include <assert.h>

#define _NTG_VECTOR_IMPLEMENTATION_
#include "shared/ntg_vector.h"
#undef _NTG_VECTOR_IMPLEMENTATION_

void __ntg_object_vec_init__(ntg_object_vec* vec)
{
    int status = __ntg_vector_init__((struct ntg_vector*)vec,
            0, sizeof(ntg_object*));

    assert(status == 0);
}

void __ntg_object_vec_deinit__(ntg_object_vec* vec)
{
    __ntg_vector_deinit__((struct ntg_vector*)vec);
}

ntg_object_vec* ntg_object_vec_new()
{
    ntg_object_vec* new = (ntg_object_vec*)malloc(
            sizeof(struct ntg_object_vec));

    if(new == NULL) return NULL;

    __ntg_object_vec_init__(new);
    
    return new;
}

void ntg_object_vec_destroy(ntg_object_vec* vec)
{
    if(vec == NULL) return;

    __ntg_object_vec_deinit__(vec);

    free(vec);
}

void ntg_object_vec_append(ntg_object_vec* vec, ntg_object* object)
{
    if(vec == NULL) return;

    int status = ntg_vector_append((struct ntg_vector*)vec,
            &object, sizeof(ntg_object*));

    assert(status == 0);
}

void ntg_object_vec_remove(ntg_object_vec* vec, ntg_object* object)
{
    if(vec == NULL) return;

    ntg_vector_remove((struct ntg_vector*)vec, &object,
            sizeof(ntg_object*), NULL);
}

ssize_t ntg_object_vec_find(const ntg_object_vec* vec, const ntg_object* object)
{
    if((vec == NULL) || (object == NULL)) return -1;

    ssize_t status = ntg_vector_find((struct ntg_vector*)vec,
            &object,
            sizeof(ntg_object*),
            NULL);

    return (status < 0) ? -1 : status;
}

bool ntg_object_vec_contains(const ntg_object_vec* vec, const ntg_object* object)
{
    return (ntg_object_vec_find(vec, object) >= 0);
}

void __ntg_object_vec_view_init__(ntg_object_vec_view* view, ntg_object_vec* vec)
{
    assert(view != NULL);
    assert(vec != NULL);

    view->__vec = vec;
}

void __ntg_object_vec_view_deinit__(ntg_object_vec_view* view)
{
    assert(view != NULL);

    view->__vec = NULL;
}

size_t ntg_object_vec_view_count(ntg_object_vec_view* view)
{
    assert(view != NULL);

    if(view->__vec == NULL) return 0;

    return view->__vec->_count;
}

ntg_object* ntg_object_vec_view_at(ntg_object_vec_view* view, size_t pos)
{
    assert(view != NULL);
    assert(view->__vec != NULL);
    assert(view->__vec->_count > pos);

    return view->__vec->_data[pos];
}
