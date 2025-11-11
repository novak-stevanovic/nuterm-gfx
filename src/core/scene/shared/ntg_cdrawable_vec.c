#include <stdlib.h>
#include <assert.h>

#include "core/scene/shared/ntg_cdrawable_vec.h"
#include "shared/_ntg_vector.h"

void __ntg_cdrawable_vec_init__(ntg_cdrawable_vec* vec)
{
    assert(vec != NULL);

    __ntg_vector_init__((ntg_vector*)vec, sizeof(ntg_drawable*), 5);
}

void __ntg_cdrawable_vec_deinit__(ntg_cdrawable_vec* vec)
{
    assert(vec != NULL);
    __ntg_vector_deinit__((ntg_vector*)vec);
}

ntg_cdrawable_vec* ntg_cdrawable_vec_new()
{
    ntg_cdrawable_vec* new = (ntg_cdrawable_vec*)malloc(sizeof(ntg_cdrawable_vec));
    assert(new != NULL);

    __ntg_cdrawable_vec_init__(new);

    return new;
}

void ntg_cdrawable_vec_destroy(ntg_cdrawable_vec* vec)
{
    assert(vec != NULL);

    __ntg_cdrawable_vec_deinit__(vec);

    free(vec);
}

void ntg_cdrawable_vec_append(ntg_cdrawable_vec* vec, const ntg_drawable* drawable)
{
    assert(vec != NULL);
    assert(drawable != NULL);

    ntg_vector_append((ntg_vector*)vec, &drawable);
}

void ntg_cdrawable_vec_remove(ntg_cdrawable_vec* vec, const ntg_drawable* drawable)
{
    assert(vec != NULL);
    assert(drawable != NULL);

    ntg_vector_remove((ntg_vector*)vec, &drawable, NULL);
}

size_t ntg_cdrawable_vec_find(const ntg_cdrawable_vec* vec,
        const ntg_drawable* drawable)
{
    assert(vec != NULL);
    assert(drawable != NULL);

    return ntg_vector_find((ntg_vector*)vec, &drawable, NULL);
}

bool ntg_cdrawable_vec_contains(const ntg_cdrawable_vec* vec,
        const ntg_drawable* drawable)
{
    assert(vec != NULL);
    assert(drawable != NULL);

    return ntg_vector_contains((ntg_vector*)vec, &drawable, NULL);
}

void __ntg_cdrawable_vec_view_init__(ntg_cdrawable_vec_view* view, ntg_cdrawable_vec* vec)
{
    assert(view != NULL);
    assert(vec != NULL);

    view->__vec = vec;
}

const ntg_cdrawable_vec_view NTG_CDRAWABLE_VEC_VIEW_EMPTY = {0};

void __ntg_cdrawable_vec_view_deinit__(ntg_cdrawable_vec_view* view)
{
    assert(view != NULL);

    view->__vec = NULL;
}

size_t ntg_cdrawable_vec_view_count(ntg_cdrawable_vec_view* view)
{
    assert(view != NULL);

    if(view->__vec == NULL) return 0;

    return view->__vec->_count;
}

const ntg_drawable* ntg_cdrawable_vec_view_at(ntg_cdrawable_vec_view* view, size_t pos)
{
    assert(view != NULL);
    assert(view->__vec != NULL);
    assert(view->__vec->_count > pos);

    return view->__vec->_data[pos];
}
