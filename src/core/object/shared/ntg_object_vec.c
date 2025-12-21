#include <stdlib.h>
#include <assert.h>

#include "core/object/shared/ntg_object_vec.h"
#include "shared/_ntg_vector.h"

void _ntg_object_vec_init_(ntg_object_vec* vec)
{
    assert(vec != NULL);

    _ntg_vector_init_((ntg_vector*)vec, sizeof(ntg_object*), 5);
}

void _ntg_object_vec_deinit_(ntg_object_vec* vec)
{
    assert(vec != NULL);
    _ntg_vector_deinit_((ntg_vector*)vec);
}

ntg_object_vec* ntg_object_vec_new()
{
    ntg_object_vec* new = (ntg_object_vec*)malloc(sizeof(ntg_object_vec));
    assert(new != NULL);

    _ntg_object_vec_init_(new);

    return new;
}

void ntg_object_vec_destroy(ntg_object_vec* vec)
{
    assert(vec != NULL);

    _ntg_object_vec_deinit_(vec);

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

size_t ntg_object_vec_find(
        const ntg_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vector_find((ntg_vector*)vec, &object, NULL);
}

bool ntg_object_vec_contains(
        const ntg_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vector_contains((ntg_vector*)vec, &object, NULL);
}

struct ntg_object_vecv ntg_object_vecv_new(ntg_object_vec* vec)
{
    assert(vec != NULL);

    return (struct ntg_object_vecv) {
        .data = vec->_data,
        .count = vec->_count
    };
}

/* -------------------------------------------------------------------------- */

void _ntg_const_object_vec_init_(ntg_const_object_vec* vec)
{
    assert(vec != NULL);

    _ntg_vector_init_((ntg_vector*)vec, sizeof(const ntg_object* const), 5);
}

void _ntg_const_object_vec_deinit_(ntg_const_object_vec* vec)
{
    assert(vec != NULL);
    _ntg_vector_deinit_((ntg_vector*)vec);
}

ntg_const_object_vec* ntg_const_object_vec_new()
{
    ntg_const_object_vec* new = (ntg_const_object_vec*)malloc(sizeof(ntg_const_object_vec));
    assert(new != NULL);

    _ntg_const_object_vec_init_(new);

    return new;
}

void ntg_const_object_vec_destroy(ntg_const_object_vec* vec)
{
    assert(vec != NULL);

    _ntg_const_object_vec_deinit_(vec);

    free(vec);
}

void ntg_const_object_vec_append(ntg_const_object_vec* vec, const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    ntg_vector_append((ntg_vector*)vec, &object);
}

void ntg_const_object_vec_remove(ntg_const_object_vec* vec, const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    ntg_vector_remove((ntg_vector*)vec, &object, NULL);
}

void ntg_const_object_vec_empty(ntg_const_object_vec* vec)
{
    assert(vec != NULL);

    ntg_vector_empty((ntg_vector*)vec);
}


size_t ntg_const_object_vec_find(
        const ntg_const_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vector_find((ntg_vector*)vec, &object, NULL);
}

bool ntg_const_object_vec_contains(
        const ntg_const_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vector_contains((ntg_vector*)vec, &object, NULL);
}

struct ntg_const_object_vecv ntg_const_object_vecv_new(ntg_const_object_vec* vec)
{
    assert(vec != NULL);

    return (struct ntg_const_object_vecv) {
        .data = vec->_data,
        .count = vec->_count
    };
}
struct ntg_const_object_vecv ntg_const_object_vecv_new_(ntg_object_vec* vec)
{
    assert(vec != NULL);

    return (struct ntg_const_object_vecv) {
        .data  = (const ntg_object * const * const)vec->_data,
        .count = vec->_count
    };
}
