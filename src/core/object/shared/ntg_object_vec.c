#include <stdlib.h>
#include <assert.h>

#include "core/object/shared/ntg_object_vec.h"
#include "shared/_ntg_vector.h"

void __ntg_object_vec_init__(ntg_object_vec* vec)
{
    assert(vec != NULL);

    __ntg_vector_init__((ntg_vector*)vec, sizeof(ntg_object*), 5);
}

void __ntg_object_vec_deinit__(ntg_object_vec* vec)
{
    assert(vec != NULL);
    __ntg_vector_deinit__((ntg_vector*)vec);
}

ntg_object_vec* ntg_object_vec_new()
{
    ntg_object_vec* new = (ntg_object_vec*)malloc(sizeof(ntg_object_vec));
    assert(new != NULL);

    __ntg_object_vec_init__(new);

    return new;
}

void ntg_object_vec_destroy(ntg_object_vec* vec)
{
    assert(vec != NULL);

    __ntg_object_vec_deinit__(vec);

    free(vec);
}

void ntg_object_vec_append(ntg_object_vec* vec, ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    ntg_vector_append((ntg_vector*)vec, &object);
}

void ntg_object_vec_remove(ntg_object_vec* vec, ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    ntg_vector_remove((ntg_vector*)vec, &object, NULL);
}

void ntg_object_vec_empty(ntg_object_vec* vec)
{
    assert(vec != NULL);

    ntg_vector_empty((ntg_vector*)vec);
}

size_t ntg_object_vec_find(const ntg_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vector_find((ntg_vector*)vec, &object, NULL);
}

bool ntg_object_vec_contains(const ntg_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vector_contains((ntg_vector*)vec, &object, NULL);
}

ntg_object_vec_view ntg_object_vec_view_new(ntg_object_vec* vec)
{
    assert(vec != NULL);

    return (ntg_object_vec_view) {
        .__vec = vec
    };
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
