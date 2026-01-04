#include "ntg.h"
#include <assert.h>
#include <stdlib.h>
#include "shared/ntg_vec.h"

static void ntg_vec_grow_if_needed(ntg_vec* vec)
{
    assert(vec != NULL);
    if (vec->_count < vec->__capacity) return;

    size_t new_cap = vec->__capacity ? vec->__capacity * 2 : 5;
    void* p = realloc(vec->__data, new_cap * vec->__data_size);
    /* assert on allocation failure per your request */
    assert(p != NULL);
    vec->__data = p;
    vec->__capacity = new_cap;
}

void ntg_vec_init(ntg_vec* vec, size_t data_size, size_t init_cap)
{
    assert(vec != NULL);
    assert(data_size > 0);
    assert(init_cap > 0);

    vec->_count = 0;
    vec->__data_size = data_size;
    vec->__capacity = init_cap; 
    vec->__data = malloc(vec->__capacity * vec->__data_size);
    assert(vec->__data != NULL);
}

void ntg_vec_deinit(ntg_vec* vec)
{
    assert(vec != NULL);
    free(vec->__data);
    vec->__data = NULL;
    vec->_count = 0;
    vec->__capacity = 0;
    vec->__data_size = 0;
}

void ntg_vec_add(ntg_vec* vec, const void* data)
{
    assert(vec != NULL);
    assert(data != NULL);

    ntg_vec_grow_if_needed(vec);

    char* base = (char*)vec->__data;
    memcpy(base + vec->_count * vec->__data_size, data, vec->__data_size);
    vec->_count += 1;
}

void ntg_vec_ins(ntg_vec* vec, size_t index, const void* data)
{
    assert(vec != NULL);
    assert(data != NULL);
    /* insing at count is allowed (add) */
    assert(index <= vec->_count);

    ntg_vec_grow_if_needed(vec);

    char* base = (char*)vec->__data;
    size_t trailing = vec->_count - index;
    if (trailing > 0)
    {
        memmove(base + (index + 1) * vec->__data_size,
                base + index * vec->__data_size,
                trailing * vec->__data_size);
    }
    memcpy(base + index * vec->__data_size, data, vec->__data_size);
    vec->_count += 1;
}

void ntg_vec_rm_at(ntg_vec* vec, size_t index)
{
    assert(vec != NULL);
    assert(vec->_count > 0);
    assert(index < vec->_count);

    char* base = (char*)vec->__data;
    size_t trailing = vec->_count - index - 1;
    if (trailing > 0)
    {
        memmove(base + index * vec->__data_size,
                base + (index + 1) * vec->__data_size,
                trailing * vec->__data_size);
    }
    vec->_count -= 1;
}

void ntg_vec_rm(ntg_vec* vec, const void* key, ntg_vec_cmp_fn cmp_fn)
{
    assert(vec != NULL);
    assert(key != NULL);

    size_t pos = ntg_vec_find(vec, key, cmp_fn);
    assert(pos != SIZE_MAX);

    ntg_vec_rm_at(vec, pos);
}

void ntg_vec_empty(ntg_vec* vec)
{
    assert(vec != NULL);

    vec->_count = 0;
}

size_t ntg_vec_find(ntg_vec* vec, const void* key, ntg_vec_cmp_fn cmp_fn)
{
    assert(vec != NULL);
    assert(key != NULL);

    size_t i;
    char* base = (char*)vec->__data;
    void* it_data;
    bool it_equal;
    for (i = 0; i < vec->_count; i++)
    {
        it_data = base + (i * vec->__data_size);
        it_equal = (cmp_fn != NULL) ?
            (cmp_fn(it_data, key) == 0):
            (memcmp(it_data, key, vec->__data_size) == 0);

        if(it_equal) return i;

    }
    return SIZE_MAX;
}

bool ntg_vec_has(ntg_vec* vec, const void* key, ntg_vec_cmp_fn cmp_fn)
{
    assert(vec != NULL);
    assert(key != NULL);

    return (ntg_vec_find(vec, key, cmp_fn) != SIZE_MAX);
}
