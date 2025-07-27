#include "base/ntg_event_sub_vec.h"
#include "base/ntg_event.h"
#include <assert.h>

#define _NTG_VECTOR_IMPLEMENTATION_
#include "shared/ntg_vector.h"
#undef _NTG_VECTOR_IMPLEMENTATION_

#define EL_SIZE sizeof(struct ntg_event_sub)

void __ntg_event_sub_vec_init__(ntg_event_sub_vec* vec)
{
    int status = __ntg_vector_init__((struct ntg_vector*)vec, 5, EL_SIZE);

    assert(status == 0);
}

void __ntg_event_sub_vec_deinit__(ntg_event_sub_vec* vec)
{
    int status = __ntg_vector_deinit__((struct ntg_vector*)vec);

    assert(status == 0);
}

ntg_event_sub_vec* ntg_event_sub_vec_new()
{
    ntg_event_sub_vec* new = (ntg_event_sub_vec*)malloc(
            sizeof(struct ntg_event_sub_vec));
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
    int status = ntg_vector_append((struct ntg_vector*)vec, &sub, EL_SIZE);

    assert(status == 0);
}

static bool _cmp_fn(const void* _sub1, const void* subscriber)
{
    const struct ntg_event_sub* sub1 = (const struct ntg_event_sub*)_sub1;

    return (sub1->subscriber == subscriber);
}

ssize_t ntg_event_sub_vec_find_sub(const ntg_event_sub_vec* vec,
        const void* subscriber)
{
    return ntg_vector_find((struct ntg_vector*)vec, subscriber,
            EL_SIZE, _cmp_fn);
}

void ntg_event_sub_vec_remove_sub(const ntg_event_sub_vec* vec,
        const void* subscriber)
{
    int status = ntg_vector_remove((struct ntg_vector*)vec,
            subscriber, EL_SIZE, _cmp_fn);

    assert(status == 0);
}
