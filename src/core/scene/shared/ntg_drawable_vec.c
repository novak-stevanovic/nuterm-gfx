#include "core/scene/shared/ntg_drawable_vec.h"
#include <assert.h>

#define _NTG_VECTOR_IMPLEMENTATION_
#include "shared/ntg_vector.h"
#undef _NTG_VECTOR_IMPLEMENTATION_

void __ntg_drawable_vec_init__(ntg_drawable_vec* vec)
{
    int status = __ntg_vector_init__((struct ntg_vector*)vec,
            0, sizeof(ntg_drawable*));

    assert(status == 0);
}

void __ntg_drawable_vec_deinit__(ntg_drawable_vec* vec)
{
    __ntg_vector_deinit__((struct ntg_vector*)vec);
}

ntg_drawable_vec* ntg_drawable_vec_new()
{
    ntg_drawable_vec* new = (ntg_drawable_vec*)malloc(
            sizeof(struct ntg_drawable_vec));

    if(new == NULL) return NULL;

    __ntg_drawable_vec_init__(new);
    
    return new;
}

void ntg_drawable_vec_destroy(ntg_drawable_vec* vec)
{
    if(vec == NULL) return;

    __ntg_drawable_vec_deinit__(vec);

    free(vec);
}

void ntg_drawable_vec_append(ntg_drawable_vec* vec, ntg_drawable* drawable)
{
    if(vec == NULL) return;

    int status = ntg_vector_append((struct ntg_vector*)vec,
            &drawable, sizeof(ntg_drawable*));

    assert(status == 0);
}

void ntg_drawable_vec_remove(ntg_drawable_vec* vec, ntg_drawable* drawable)
{
    if(vec == NULL) return;

    ntg_vector_remove((struct ntg_vector*)vec, &drawable,
            sizeof(ntg_drawable*), NULL);
}

ssize_t ntg_drawable_vec_find(const ntg_drawable_vec* vec, const ntg_drawable* drawable)
{
    if((vec == NULL) || (drawable == NULL)) return -1;
    if(vec->_data == NULL) return -1;

    ssize_t status = ntg_vector_find((struct ntg_vector*)vec,
            &drawable,
            sizeof(ntg_drawable*),
            NULL);

    return (status < 0) ? -1 : status;
}

bool ntg_drawable_vec_contains(const ntg_drawable_vec* vec, const ntg_drawable* drawable)
{
    if(vec->_data == NULL) return false;

    return (ntg_drawable_vec_find(vec, drawable) >= 0);
}

void __ntg_drawable_vec_view_init__(ntg_drawable_vec_view* view, ntg_drawable_vec* vec)
{
    assert(view != NULL);
    assert(vec != NULL);

    view->__vec = vec;
}

const ntg_drawable_vec_view NTG_DRAWABLE_VEC_VIEW_EMPTY = {0};

void __ntg_drawable_vec_view_deinit__(ntg_drawable_vec_view* view)
{
    assert(view != NULL);

    view->__vec = NULL;
}

size_t ntg_drawable_vec_view_count(ntg_drawable_vec_view* view)
{
    assert(view != NULL);

    if(view->__vec == NULL) return 0;

    return view->__vec->_count;
}

ntg_drawable* ntg_drawable_vec_view_at(ntg_drawable_vec_view* view, size_t pos)
{
    assert(view != NULL);
    assert(view->__vec != NULL);
    assert(view->__vec->_count > pos);

    return view->__vec->_data[pos];
}
