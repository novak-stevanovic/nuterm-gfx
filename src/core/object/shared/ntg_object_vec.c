#include "ntg.h"
#include "shared/ntg_vec.h"
#include <assert.h>
#include <stdlib.h>

void ntg_object_vec_init(ntg_object_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_init((ntg_vec*)vec, sizeof(ntg_object*), 5);
}

void _ntg_object_vec_deinit_(ntg_object_vec* vec)
{
    assert(vec != NULL);
    ntg_vec_deinit((ntg_vec*)vec);
}

void ntg_object_vec_add(ntg_object_vec* vec, ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    ntg_vec_add((ntg_vec*)vec, &object);
}

void ntg_object_vec_rm(ntg_object_vec* vec, ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    ntg_vec_rm((ntg_vec*)vec, &object, NULL);
}

void ntg_object_vec_empty(ntg_object_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_empty((ntg_vec*)vec);
}

size_t ntg_object_vec_find(
        const ntg_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vec_find((ntg_vec*)vec, &object, NULL);
}

bool ntg_object_vec_has(
        const ntg_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vec_has((ntg_vec*)vec, &object, NULL);
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

void ntg_const_object_vec_init(ntg_const_object_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_init((ntg_vec*)vec, sizeof(const ntg_object* const), 5);
}

void _ntg_const_object_vec_deinit_(ntg_const_object_vec* vec)
{
    assert(vec != NULL);
    ntg_vec_deinit((ntg_vec*)vec);
}

void ntg_const_object_vec_add(ntg_const_object_vec* vec, const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    ntg_vec_add((ntg_vec*)vec, &object);
}

void ntg_const_object_vec_rm(ntg_const_object_vec* vec, const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    ntg_vec_rm((ntg_vec*)vec, &object, NULL);
}

void ntg_const_object_vec_empty(ntg_const_object_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_empty((ntg_vec*)vec);
}


size_t ntg_const_object_vec_find(
        const ntg_const_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vec_find((ntg_vec*)vec, &object, NULL);
}

bool ntg_const_object_vec_has(
        const ntg_const_object_vec* vec,
        const ntg_object* object)
{
    assert(vec != NULL);
    assert(object != NULL);

    return ntg_vec_has((ntg_vec*)vec, &object, NULL);
}

struct ntg_const_object_vecv ntg_const_object_vecv_new(ntg_const_object_vec* vec)
{
    assert(vec != NULL);

    return (struct ntg_const_object_vecv) {
        .data = vec->_data,
        .count = vec->_count
    };
}
struct ntg_const_object_vecv ntg_const_object_vecv_new_(const ntg_object_vec* vec)
{
    assert(vec != NULL);

    return (struct ntg_const_object_vecv) {
        .data  = (const ntg_object * const *)vec->_data,
        .count = vec->_count
    };
}
