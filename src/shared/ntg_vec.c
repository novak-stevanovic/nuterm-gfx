#include "ntg.h"
#include <assert.h>
#include <stdlib.h>
#include "shared/ntg_vec.h"

static void ntg_vec_grow_if_needed(ntg_vec* vec, size_t data_size)
{
    if (vec->_count < vec->__capacity) return;

    size_t new_cap = vec->__capacity ? vec->__capacity * vec->__grow_factor : 5;
    void* p = realloc(vec->_data, new_cap * data_size);
    /* assert on allocation failure per your request */
    assert(p != NULL);
    vec->_data = p;
    vec->__capacity = new_cap;
}

void ntg_vec_init(ntg_vec* vec, size_t data_size, size_t init_cap, size_t grow_factor)
{
    assert(vec != NULL);
    assert(data_size > 0);
    assert(init_cap > 0);
    assert(grow_factor > 0);

    vec->_count = 0;
    vec->__capacity = init_cap; 
    vec->_data = malloc(vec->__capacity * data_size);
    vec->__grow_factor = grow_factor;
    assert(vec->_data != NULL);
}

void ntg_vec_deinit(ntg_vec* vec)
{
    assert(vec != NULL);
    free(vec->_data);
    vec->_data = NULL;
    vec->_count = 0;
    vec->__capacity = 0;
    vec->__grow_factor = 0;
}

void ntg_vec_add(ntg_vec* vec, const void* data, size_t data_size)
{
    assert(vec != NULL);
    assert(data != NULL);
    assert(data_size > 0);

    ntg_vec_grow_if_needed(vec, data_size);

    char* base = (char*)vec->_data;
    memcpy(base + vec->_count * data_size, data, data_size);
    vec->_count += 1;
}

void ntg_vec_ins(ntg_vec* vec, size_t index, const void* data, size_t data_size)
{
    assert(vec != NULL);
    assert(data != NULL);
    /* insing at count is allowed (add) */
    assert(index <= vec->_count);
    assert(data_size > 0);

    ntg_vec_grow_if_needed(vec, data_size);

    char* base = (char*)vec->_data;
    size_t trailing = vec->_count - index;
    if (trailing > 0)
    {
        memmove(base + (index + 1) * data_size,
                base + index * data_size,
                trailing * data_size);
    }
    memcpy(base + index * data_size, data, data_size);
    vec->_count += 1;
}

void ntg_vec_rm_at(ntg_vec* vec, size_t index, size_t data_size)
{
    assert(vec != NULL);
    assert(vec->_count > 0);
    assert(index < vec->_count);
    assert(data_size > 0);

    char* base = (char*)vec->_data;
    size_t trailing = vec->_count - index - 1;
    if (trailing > 0)
    {
        memmove(base + index * data_size,
                base + (index + 1) * data_size,
                trailing * data_size);
    }
    vec->_count -= 1;
}

void ntg_vec_rm(ntg_vec* vec, const void* key, ntg_vec_cmp_fn cmp_fn, size_t data_size)
{
    assert(vec != NULL);
    assert(key != NULL);
    assert(data_size > 0);

    size_t pos = ntg_vec_find(vec, key, cmp_fn, data_size);
    assert(pos != SIZE_MAX);

    ntg_vec_rm_at(vec, pos, data_size);
}

void ntg_vec_empty(ntg_vec* vec)
{
    assert(vec != NULL);

    vec->_count = 0;
}

void ntg_vec_shrink(ntg_vec* vec, size_t data_size)
{
    assert(vec != NULL);
    assert(data_size > 0);

    vec->_data = realloc(vec->_data, data_size * vec->_count);
    assert(vec->_data != NULL);
}

size_t ntg_vec_find(ntg_vec* vec, const void* key,
        ntg_vec_cmp_fn cmp_fn, size_t data_size)
{
    assert(vec != NULL);
    assert(key != NULL);
    assert(data_size > 0);

    size_t i;
    char* base = (char*)vec->_data;
    void* it_data;
    bool it_equal;
    for (i = 0; i < vec->_count; i++)
    {
        it_data = base + (i * data_size);
        it_equal = (cmp_fn != NULL) ?
            (cmp_fn(it_data, key) == 0):
            (memcmp(it_data, key, data_size) == 0);

        if(it_equal) return i;

    }
    return SIZE_MAX;
}

bool ntg_vec_has(ntg_vec* vec, const void* key,
        ntg_vec_cmp_fn cmp_fn, size_t data_size)
{
    assert(vec != NULL);
    assert(key != NULL);
    assert(data_size > 0);

    return (ntg_vec_find(vec, key, cmp_fn, data_size) != SIZE_MAX);
}
