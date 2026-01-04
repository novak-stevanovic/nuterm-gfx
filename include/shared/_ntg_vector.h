#ifndef __NTG_VECTOR_H__
#define __NTG_VECTOR_H__

#include "shared/ntg_typedef.h"

struct ntg_vector
{
    void* __data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
};

void _ntg_vector_init_(ntg_vector* v, size_t data_size, size_t init_cap);
void _ntg_vector_deinit_(ntg_vector* vector);

void ntg_vector_append(ntg_vector* vector, const void* data);
void ntg_vector_insert(ntg_vector* vector, size_t index, const void* data);

void ntg_vector_remove_at(ntg_vector* vector, size_t index);
void ntg_vector_remove(ntg_vector* vector, const void* key,
        int (*cmp_fn)(const void* data1, const void* data2));
void ntg_vector_empty(ntg_vector* vector);

/* Returns index in [0, count-1] if found, or SIZE_MAX if not found. Uses default memcpy to compare values. */
size_t ntg_vector_find(
        ntg_vector* vector,
        const void* key,
        int (*cmp_fn)(const void* it_data, const void* key));
bool ntg_vector_contains(
        ntg_vector* vector,
        const void* key,
        int (*cmp_fn)(const void* it_data, const void* key));

#endif // __NTG_VECTOR_H__
