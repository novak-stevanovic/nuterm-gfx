#ifndef _NTG_VECTOR_H_
#define _NTG_VECTOR_H_

#include <stddef.h>
#include <sys/types.h>

struct ntg_vector
{
    size_t count;
    size_t capacity;
    void* data;
};

// #define _NTG_VECTOR_IMPLEMENTATION_
#ifdef _NTG_VECTOR_IMPLEMENTATION_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Functions below return:
 * 0 on success,
 * 1 if one or more of the provided arguments is invalid,
 * 2 if malloc()/realloc() failed.
 * 
 * Additionally, ntg_vector_remove() returns 3 if `data` is not found in vector. */

static int __ntg_vector_init__(struct ntg_vector* vec,
        size_t init_cap, size_t el_size)
{
    if((vec == NULL) || (el_size == 0)) return 1;

    if(init_cap != 0)
    {
        vec->data = malloc(init_cap * el_size);
        if(vec->data == NULL) return 2;
    }
    else
        vec->data = NULL;

    vec->capacity = init_cap;
    vec->count = 0;
    
    return 0;
}

static int __ntg_vector_deinit__(struct ntg_vector* vec)
{
    if(vec == NULL) return 1;

    vec->count = 0;
    vec->capacity = 0;
    if(vec->data != NULL)
        free(vec->data);

    vec->data = NULL;
    return 0;
}

static int ntg_vector_insert(struct ntg_vector* vec, const void* data,
        size_t pos, size_t el_size)
{
    if((vec == NULL) || (el_size == 0) || (data == NULL) || (pos > vec->count))
        return 1;

    if(vec->count >= vec->capacity)
    {
        size_t new_cap = (vec->count > 0) ? (vec->count * 2) : 1;
        void* new_data = realloc(vec->data, el_size * new_cap);
        if(new_data == NULL) return 2;

        vec->capacity = new_cap;
        vec->data = new_data;
    }

    char* _vec_data = (char*)vec->data;
    
    memmove(_vec_data + ((pos + 1) * el_size),
            _vec_data + (pos * el_size),
            (vec->count - pos) * el_size);

    memcpy(_vec_data + (pos * el_size), data, el_size);

    vec->count++;

    return 0;
}

static int ntg_vector_append(struct ntg_vector* vec, const void* data,
        size_t el_size)

{
    return ntg_vector_insert(vec, data, vec->count, el_size);
}

static int ntg_vector_remove_at(struct ntg_vector* vec, size_t pos,
        size_t el_size)
{
    if((vec == NULL) || (el_size == 0) || (pos >= vec->count))
        return 1;

    char* _vec_data = (char*)vec->data;

    memmove(_vec_data + (pos * el_size),
            _vec_data + ((pos + 1) * el_size),
            (vec->count - pos - 1) * el_size);

    vec->count--;

    return 0;
}

static ssize_t ntg_vector_find(const struct ntg_vector* vec, const void* data,
        size_t el_size, bool (*cmp_func)(const void*, const void*))
{
    if((vec == NULL) || (data == NULL) || (el_size == 0))
        return -2;

    char* _vec_data = (char*)vec->data;

    size_t i;
    void* it;
    int it_res;
    if(cmp_func != NULL)
    {
        for(i = 0; i < vec->count; i++)
        {
            it = _vec_data + (i * el_size);

            it_res = cmp_func(it, data);
            if(it_res == 1)
                return i;
        }
    }
    else
    {
        for(i = 0; i < vec->count; i++)
        {
            it = _vec_data + (i * el_size);

            it_res = memcmp(it, data, el_size);
            if(it_res == 0)
                return i;
        }
    }

    return -1;
}

static int ntg_vector_remove(struct ntg_vector* vec, const void* data,
        size_t el_size, bool (*cmp_func)(const void*, const void*))
{
    if((vec == NULL) || (data == NULL) || (el_size == 0))
        return 1;

    ssize_t find_res = ntg_vector_find(vec, data, el_size, cmp_func);

    if(find_res == -1) return 3;

    ntg_vector_remove_at(vec, find_res, el_size);

    return 0;
}

#endif // _NTG_VECTOR_IMPLEMENTATION_

#endif // _NTG_VECTOR_H_
