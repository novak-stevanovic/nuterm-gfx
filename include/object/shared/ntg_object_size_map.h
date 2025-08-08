#ifndef _NTG_OBJECT_SIZE_MAP_H_
#define _NTG_OBJECT_SIZE_MAP_H_

#include <stddef.h>

typedef struct ntg_object ntg_object;

struct ntg_object_size
{
    ntg_object* object;
    size_t size;
};

typedef struct ntg_object_size_map
{
    struct ntg_object_positions* __data;
} ntg_object_size_map;

void __ntg_object_size_map_init__(ntg_object_size_map* map, ntg_object* parent);
void __ntg_object_size_map_deinit__(ntg_object_size_map* map);

void ntg_object_size_map_set(ntg_object_size_map* map, ntg_object* child,
        size_t size);

size_t ntg_object_size_map_get(const ntg_object_size_map* map,
        const ntg_object* object);

#endif // _NTG_OBJECT_SIZE_MAP_H_
