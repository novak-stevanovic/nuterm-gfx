#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "shared/_ntg_vec_map.h"
#include "shared/_ntg_vector.h"

void __ntg_vec_map_init__(
        ntg_vec_map* map,
        size_t key_size,
        size_t value_size,
        int (*key_cmp_fn)(const void* it_pair, const void* key))
{
    assert(map != NULL);
    assert(key_size > 0);
    assert(value_size > 0);
    assert(key_cmp_fn != NULL);

    (*map) = (ntg_vec_map) {0};
    
    __ntg_vector_init__((ntg_vector*)map, sizeof(struct ntg_vec_map_pair), 5);

    map->__key_size = key_size;
    map->__value_size = value_size;
    map->__key_cmp_fn =  key_cmp_fn;
}

void __ntg_vec_map_deinit__(ntg_vec_map* map)
{
    assert(map != NULL);

    size_t i;
    struct ntg_vec_map_pair* it_pair;
    for(i = 0; i < map->_count; i++)
    {
        it_pair = &(map->__data[i]);
        free(it_pair->key);
        free(it_pair->value);

        it_pair->key = NULL;
        it_pair->value = NULL;
    }

    __ntg_vector_deinit__((ntg_vector*)map);

    (*map) = (ntg_vec_map) {0};
}

void ntg_vec_map_add(ntg_vec_map* map, const void* key, const void* value)
{
    assert(map != NULL);
    assert(key != NULL);
    assert(value != NULL);

    void* found = ntg_vec_map_get(map, key);
    assert(found == NULL);

    void* key_cpy = malloc(map->__key_size);
    void* value_cpy = malloc(map->__value_size);
    assert(key_cpy != NULL);
    assert(value_cpy != NULL);

    memcpy(key_cpy, key, map->__key_size);
    memcpy(value_cpy, value, map->__value_size);

    struct ntg_vec_map_pair new = {
        .key = key_cpy,
        .value = value_cpy
    };

    ntg_vector_append((ntg_vector*)map, &new);
}

void ntg_vec_map_remove(ntg_vec_map* map, const void* key)
{
    assert(map != NULL);
    assert(key != NULL);

    size_t idx = ntg_vector_find((ntg_vector*)map, key, map->__key_cmp_fn);
    assert(idx != SIZE_MAX);

    struct ntg_vec_map_pair* pair = &(map->__data[idx]);
    void* key_cpy_ptr = pair->key;
    void* value_cpy_ptr = pair->value;

    ntg_vector_remove_at((ntg_vector*)map, idx);

    free(key_cpy_ptr);
    free(value_cpy_ptr);
}

void* ntg_vec_map_get(const ntg_vec_map* map, const void* key)
{
    assert(map != NULL);
    assert(key != NULL);

    size_t idx = ntg_vector_find((ntg_vector*)map, key, map->__key_cmp_fn);

    return (idx != SIZE_MAX) ? map->__data[idx].value : NULL;
}
