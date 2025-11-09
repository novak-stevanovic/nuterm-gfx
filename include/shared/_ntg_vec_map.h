#ifndef __NTG_VEC_MAP_H__
#define __NTG_VEC_MAP_H__

#include <stddef.h>

typedef struct ntg_vec_map ntg_vec_map;

struct ntg_vec_map_pair
{
    void *key, *value;
};

struct ntg_vec_map
{
    struct ntg_vec_map_pair* __data;
    size_t _count;
    size_t _capacity;
    size_t __data_size;

    size_t __key_size, __value_size;
    int (*__key_cmp_fn)(const void* it_pair, const void* key);
};

void __ntg_vec_map_init__(
        ntg_vec_map* map,
        size_t key_size,
        size_t value_size,
        int (*key_cmp_fn)(const void* it_pair, const void* key));
void __ntg_vec_map_deinit__(ntg_vec_map* map);

/* Creates copies of `key` and `value`. */
void ntg_vec_map_add(ntg_vec_map* map, const void* key, const void* value);

/* Frees copies of `key` and `value`. */
void ntg_vec_map_remove(ntg_vec_map* map, const void* key);

/* Returns ptr to value copy */
void* ntg_vec_map_get(const ntg_vec_map* map, const void* key);

#endif // __NTG_VEC_MAP_H__
