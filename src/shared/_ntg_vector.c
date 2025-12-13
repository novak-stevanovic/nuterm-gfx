#include "shared/_ntg_vector.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h> 

static void ntg_vector_grow_if_needed(ntg_vector* vector)
{
    assert(vector != NULL);
    if (vector->_count < vector->__capacity) return;

    size_t new_cap = vector->__capacity ? vector->__capacity * 2 : 4;
    void* p = realloc(vector->__data, new_cap * vector->__data_size);
    /* assert on allocation failure per your request */
    assert(p != NULL);
    vector->__data = p;
    vector->__capacity = new_cap;
}

void _ntg_vector_init_(ntg_vector* vector, size_t data_size, size_t init_cap)
{
    assert(vector != NULL);
    assert(data_size > 0);
    assert(init_cap > 0 );

    vector->_count = 0;
    vector->__data_size = data_size;
    vector->__capacity = init_cap ? init_cap : 4;
    vector->__data = malloc(vector->__capacity * vector->__data_size);
    assert(vector->__data != NULL);
}

void _ntg_vector_deinit_(ntg_vector* vector)
{
    assert(vector != NULL);
    free(vector->__data);
    vector->__data = NULL;
    vector->_count = 0;
    vector->__capacity = 0;
    vector->__data_size = 0;
}

void ntg_vector_append(ntg_vector* vector, const void* data)
{
    assert(vector != NULL);
    assert(data != NULL);

    ntg_vector_grow_if_needed(vector);

    char* base = (char*)vector->__data;
    memcpy(base + vector->_count * vector->__data_size, data, vector->__data_size);
    vector->_count += 1;
}

void ntg_vector_insert(ntg_vector* vector, size_t index, const void* data)
{
    assert(vector != NULL);
    assert(data != NULL);
    /* inserting at count is allowed (append) */
    assert(index <= vector->_count);

    ntg_vector_grow_if_needed(vector);

    char* base = (char*)vector->__data;
    size_t trailing = vector->_count - index;
    if (trailing > 0)
    {
        memmove(base + (index + 1) * vector->__data_size,
                base + index * vector->__data_size,
                trailing * vector->__data_size);
    }
    memcpy(base + index * vector->__data_size, data, vector->__data_size);
    vector->_count += 1;
}

void ntg_vector_remove_at(ntg_vector* vector, size_t index)
{
    assert(vector != NULL);
    assert(vector->_count > 0);
    assert(index < vector->_count);

    char* base = (char*)vector->__data;
    size_t trailing = vector->_count - index - 1;
    if (trailing > 0)
    {
        memmove(base + index * vector->__data_size,
                base + (index + 1) * vector->__data_size,
                trailing * vector->__data_size);
    }
    vector->_count -= 1;
}

void ntg_vector_remove(ntg_vector* vector, const void* key,
        int (*cmp_fn)(const void* data1, const void* data2))
{
    assert(vector != NULL);
    assert(key != NULL);

    size_t pos = ntg_vector_find(vector, key, cmp_fn);
    assert(pos != SIZE_MAX);

    ntg_vector_remove_at(vector, pos);
}

void ntg_vector_empty(ntg_vector* vector)
{
    assert(vector != NULL);

    vector->_count = 0;
}

size_t ntg_vector_find(
        ntg_vector* vector,
        const void* key,
        int (*cmp_fn)(const void* it_data, const void* key))
{
    assert(vector != NULL);
    assert(key != NULL);

    size_t i;
    char* base = (char*)vector->__data;
    void* it_data;
    bool it_equal;
    for (i = 0; i < vector->_count; i++)
    {
        it_data = base + (i * vector->__data_size);
        it_equal = (cmp_fn != NULL) ?
            (cmp_fn(it_data, key) == 0):
            (memcmp(it_data, key, vector->__data_size) == 0);

        if(it_equal) return i;

    }
    return SIZE_MAX;
}

bool ntg_vector_contains(
        ntg_vector* vector,
        const void* key,
        int (*cmp_fn)(const void* it_data, const void* key))
{
    assert(vector != NULL);
    assert(key != NULL);

    return (ntg_vector_find(vector, key, cmp_fn) != SIZE_MAX);
}
