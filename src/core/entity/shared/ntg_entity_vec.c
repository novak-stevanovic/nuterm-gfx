#include "core/entity/shared/ntg_entity_vec.h"
#include "ntg.h"
#include "shared/ntg_vec.h"

#include <assert.h>
#include <stdlib.h>

void ntg_entity_vec_init(ntg_entity_vec* vec)
{
    assert(vec != NULL);
    ntg_vec_init((ntg_vec*)vec, sizeof(ntg_entity*), 5, 2);
}

void ntg_entity_vec_deinit(ntg_entity_vec* vec)
{
    assert(vec != NULL);
    ntg_vec_deinit((ntg_vec*)vec);
}

void ntg_entity_vec_add(ntg_entity_vec* vec, ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    ntg_vec_add((ntg_vec*)vec, &entity, sizeof(ntg_entity*));
}

void ntg_entity_vec_rm(ntg_entity_vec* vec, ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    ntg_vec_rm((ntg_vec*)vec, &entity, NULL, sizeof(ntg_entity*));
}

void ntg_entity_vec_empty(ntg_entity_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_empty((ntg_vec*)vec);
}

size_t ntg_entity_vec_find(const ntg_entity_vec* vec, const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    return ntg_vec_find((ntg_vec*)vec, &entity, NULL, sizeof(ntg_entity*));
}

bool ntg_entity_vec_has(const ntg_entity_vec* vec, const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    return ntg_vec_has((ntg_vec*)vec, &entity, NULL, sizeof(ntg_entity*));
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

void ntg_const_entity_vec_init(ntg_const_entity_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_init((ntg_vec*)vec, sizeof(const ntg_entity* const), 5, sizeof(ntg_entity*));
}

void ntg_const_entity_vec_deinit(ntg_const_entity_vec* vec)
{
    assert(vec != NULL);
    ntg_vec_deinit((ntg_vec*)vec);
}

void ntg_const_entity_vec_add(ntg_const_entity_vec* vec, const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    ntg_vec_add((ntg_vec*)vec, &entity, sizeof(ntg_entity*));
}

void ntg_const_entity_vec_rm(ntg_const_entity_vec* vec, const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    ntg_vec_rm((ntg_vec*)vec, &entity, NULL, sizeof(ntg_entity*));
}

void ntg_const_entity_vec_empty(ntg_const_entity_vec* vec)
{
    assert(vec != NULL);

    ntg_vec_empty((ntg_vec*)vec);
}

size_t ntg_const_entity_vec_find(const ntg_const_entity_vec* vec, const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    return ntg_vec_find((ntg_vec*)vec, &entity, NULL, sizeof(ntg_entity*));
}

bool ntg_const_entity_vec_has(const ntg_const_entity_vec* vec, const ntg_entity* entity)
{
    assert(vec != NULL);
    assert(entity != NULL);

    return ntg_vec_has((ntg_vec*)vec, &entity, NULL, sizeof(ntg_entity*));
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
