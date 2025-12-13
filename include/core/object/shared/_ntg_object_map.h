#ifndef __NTG_MAP_H__
#define __NTG_MAP_H__

#include <stddef.h>

typedef struct ntg_object_map ntg_object_map;
typedef struct ntg_object ntg_object;
typedef struct sarena sarena;

struct ntg_object_map
{
    const ntg_object** __keys;
    char* __values;
    size_t __data_size, __count, __capacity;
};

void _ntg_object_map_init_(ntg_object_map* ctx,
        size_t capacity, size_t data_size, sarena* arena);

void ntg_object_map_set(ntg_object_map* ctx, const ntg_object* object, void* data);
void* ntg_object_map_get(const ntg_object_map* ctx, const ntg_object* object);

#endif // __NTG_MAP_H__
