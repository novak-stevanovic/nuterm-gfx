#include "ntg.h"

#include "base/entity/shared/ntg_entity_vec.h"
#include "shared/_ntg_vector.h"
#include <assert.h>
#include <stdlib.h>

void _ntg_entity_vec_init_(ntg_entity_vec* vec)
{
    assert(vec != NULL);

    _ntg_vector_init_((ntg_vector*)vec, sizeof(ntg_entity*), 5);
}

void _ntg_entity_vec_deinit_(ntg_entity_vec* vec)
{
    assert(vec != NULL);
    _ntg_vector_deinit_((ntg_vector*)vec);
}

ntg_entity_vec* ntg_entity_vec_new()
{
    ntg_entity_vec* new = (ntg_entity_vec*)malloc(sizeof(ntg_entity_vec));
    assert(new != NULL);

    _ntg_entity_vec_init_(new);

    return new;
}

void ntg_entity_vec_destroy(ntg_entity_vec* vec)
{
    assert(vec != NULL);

    _ntg_entity_vec_deinit_(vec);

    free(vec);
}

void ntg_entity_vec_append(ntg_entity_vec* vec, ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    ntg_vector_append((ntg_vector*)vec, &entity);
}

void ntg_entity_vec_remove(ntg_entity_vec* vec, ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    ntg_vector_remove((ntg_vector*)vec, &entity, NULL);
}

void ntg_entity_vec_empty(ntg_entity_vec* vec)
{
    assert(vec != NULL);

    ntg_vector_empty((ntg_vector*)vec);
}

size_t ntg_entity_vec_find(
        const ntg_entity_vec* vec,
        const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    return ntg_vector_find((ntg_vector*)vec, &entity, NULL);
}

bool ntg_entity_vec_contains(
        const ntg_entity_vec* vec,
        const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    return ntg_vector_contains((ntg_vector*)vec, &entity, NULL);
}

struct ntg_entity_vecv ntg_entity_vecv_new(ntg_entity_vec* vec)
{
    assert(vec != NULL);

    return (struct ntg_entity_vecv) {
        .data = vec->_data,
        .count = vec->_count
    };
}

/* -------------------------------------------------------------------------- */

void _ntg_const_entity_vec_init_(ntg_const_entity_vec* vec)
{
    assert(vec != NULL);

    _ntg_vector_init_((ntg_vector*)vec, sizeof(const ntg_entity* const), 5);
}

void _ntg_const_entity_vec_deinit_(ntg_const_entity_vec* vec)
{
    assert(vec != NULL);
    _ntg_vector_deinit_((ntg_vector*)vec);
}

ntg_const_entity_vec* ntg_const_entity_vec_new()
{
    ntg_const_entity_vec* new = (ntg_const_entity_vec*)malloc(sizeof(ntg_const_entity_vec));
    assert(new != NULL);

    _ntg_const_entity_vec_init_(new);

    return new;
}

void ntg_const_entity_vec_destroy(ntg_const_entity_vec* vec)
{
    assert(vec != NULL);

    _ntg_const_entity_vec_deinit_(vec);

    free(vec);
}

void ntg_const_entity_vec_append(ntg_const_entity_vec* vec, const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    ntg_vector_append((ntg_vector*)vec, &entity);
}

void ntg_const_entity_vec_remove(ntg_const_entity_vec* vec, const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    ntg_vector_remove((ntg_vector*)vec, &entity, NULL);
}

void ntg_const_entity_vec_empty(ntg_const_entity_vec* vec)
{
    assert(vec != NULL);

    ntg_vector_empty((ntg_vector*)vec);
}


size_t ntg_const_entity_vec_find(
        const ntg_const_entity_vec* vec,
        const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    return ntg_vector_find((ntg_vector*)vec, &entity, NULL);
}

bool ntg_const_entity_vec_contains(
        const ntg_const_entity_vec* vec,
        const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    return ntg_vector_contains((ntg_vector*)vec, &entity, NULL);
}

struct ntg_const_entity_vecv ntg_const_entity_vecv_new(ntg_const_entity_vec* vec)
{
    assert(vec != NULL);

    return (struct ntg_const_entity_vecv) {
        .data = vec->_data,
        .count = vec->_count
    };
}
struct ntg_const_entity_vecv ntg_const_entity_vecv_new_(ntg_entity_vec* vec)
{
    assert(vec != NULL);

    return (struct ntg_const_entity_vecv) {
        .data  = (const ntg_entity * const *)vec->_data,
        .count = vec->_count
    };
}
