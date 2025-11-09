#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "base/event/ntg_event_sub_vec.h"
#include "base/event/ntg_event.h"
#include "shared/_ntg_vector.h"

static int __cmp_fn(const void* _it_data, const void* key)
{
    assert(_it_data != NULL);
    assert(key != NULL);

    struct ntg_event_sub* it_data = (struct ntg_event_sub*)_it_data;

    return memcmp(it_data, key, sizeof(void*));
}

void __ntg_event_sub_vec_init__(ntg_event_sub_vec* vec)
{
    assert(vec != NULL);

    __ntg_vector_init__((ntg_vector*)vec, sizeof(struct ntg_event_sub), 5);
}

void __ntg_event_sub_vec_deinit__(ntg_event_sub_vec* vec)
{
    assert(vec != NULL);

    __ntg_vector_deinit__((ntg_vector*)vec);
}

ntg_event_sub_vec* ntg_event_sub_vec_new()
{

    ntg_event_sub_vec* new = (ntg_event_sub_vec*)malloc(
            sizeof(ntg_event_sub_vec));
    assert(new != NULL);

    __ntg_event_sub_vec_init__(new);

    return new;
}

void ntg_event_sub_vec_destroy(ntg_event_sub_vec* vec)
{
    assert(vec != NULL);

    __ntg_event_sub_vec_deinit__(vec);
    free(vec);
}

void ntg_event_sub_vec_append(ntg_event_sub_vec* vec, struct ntg_event_sub sub)
{
    assert(vec != NULL);

    assert(!ntg_event_sub_vec_contains(vec, sub.subscriber));

    ntg_vector_append((ntg_vector*)vec, &sub);
}

void ntg_event_sub_vec_remove(ntg_event_sub_vec* vec, struct ntg_event_sub sub)
{
    assert(vec != NULL);

    assert(ntg_event_sub_vec_contains(vec, sub.subscriber));

    ntg_vector_remove((ntg_vector*)vec, &sub, __cmp_fn);
}

size_t ntg_event_sub_vec_find(const ntg_event_sub_vec* vec,
        const void* subscriber)
{
    assert(vec != NULL);
    assert(subscriber != NULL);

    return ntg_vector_find((ntg_vector*)vec, subscriber, __cmp_fn);
}

bool ntg_event_sub_vec_contains(const ntg_event_sub_vec* vec,
        const void* subscriber)
{
    assert(vec != NULL);
    assert(subscriber != NULL);

    return ntg_vector_contains((ntg_vector*)vec, subscriber, __cmp_fn);
}
