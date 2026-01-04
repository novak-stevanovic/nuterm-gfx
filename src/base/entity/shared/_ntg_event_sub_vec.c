#include "ntg.h"
#include "base/entity/shared/_ntg_event_sub_vec.h"
#include "shared/_ntg_vector.h"
#include <stdlib.h>

struct _ntg_event_sub
{
    const ntg_entity* entity;
    ntg_event_handler_fn handler;
};

void _ntg_event_sub_vec_init_(ntg_event_sub_vec* vec)
{
    assert(vec != NULL);

    _ntg_vector_init_((ntg_vector*)vec, sizeof(struct ntg_event_sub), 5);
}

void __ntg_event_sub_vec_deinit__(ntg_event_sub_vec* vec)
{
    assert(vec != NULL);

    _ntg_vector_deinit_((ntg_vector*)vec);
}

ntg_event_sub_vec* ntg_event_sub_vec_new()
{
    ntg_event_sub_vec* new = (ntg_event_sub_vec*)malloc(
            sizeof(ntg_event_sub_vec));
    assert(new != NULL);

    _ntg_event_sub_vec_init_(new);

    return new;
}

void ntg_event_sub_vec_destroy(ntg_event_sub_vec* vec)
{
    assert(vec != NULL);

    __ntg_event_sub_vec_deinit__(vec);
    free(vec);
}

void ntg_event_sub_vec_append(
        ntg_event_sub_vec* vec,
        ntg_entity* observer,
        ntg_event_handler_fn handler)
{
    assert(vec != NULL);
    assert(observer != NULL);
    assert(handler != NULL);

    struct ntg_event_sub sub = {
        .entity = observer,
        .handler = handler
    };

    ntg_vector_append((ntg_vector*)vec, &sub);
}

void ntg_event_sub_vec_insert(
        ntg_event_sub_vec* vec,
        ntg_entity* observer,
        ntg_event_handler_fn handler,
        size_t pos)
{
    assert(vec != NULL);
    assert(observer != NULL);
    assert(handler != NULL);
    assert(pos <= vec->_count);

    struct ntg_event_sub sub = {
        .entity = observer,
        .handler = handler
    };

    ntg_vector_insert((ntg_vector*)vec, pos, &sub);
}

void ntg_event_sub_vec_remove(
        ntg_event_sub_vec* vec,
        ntg_entity* observer,
        ntg_event_handler_fn handler)
{
    assert(vec != NULL);
    assert(observer != NULL);
    assert(handler != NULL);

    struct ntg_event_sub sub = {
        .entity = observer,
        .handler = handler
    };

    ntg_vector_remove((ntg_vector*)vec, &sub, NULL);
}

size_t ntg_event_sub_vec_find(
        const ntg_event_sub_vec* vec,
        const ntg_entity* observer,
        ntg_event_handler_fn handler)
{
    assert(vec != NULL);
    assert(observer != NULL);
    assert(handler != NULL);

    struct _ntg_event_sub sub = {
        .entity = observer,
        .handler = handler
    };

    return ntg_vector_find((ntg_vector*)vec, &sub, NULL);
}

bool ntg_event_sub_vec_contains(
        const ntg_event_sub_vec* vec,
        const ntg_entity* observer,
        ntg_event_handler_fn handler)
{
    assert(vec != NULL);
    assert(observer != NULL);
    assert(handler != NULL);

    struct _ntg_event_sub sub = {
        .entity = observer,
        .handler = handler
    };

    return ntg_vector_contains((ntg_vector*)vec, &sub, NULL);
}
