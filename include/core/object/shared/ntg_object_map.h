#ifndef NTG_OBJECT_MAP_H
#define NTG_OBJECT_MAP_H

#include "shared/ntg_typedef.h"

struct ntg_object_map
{
    const ntg_object** __keys;
    char* __values;
    size_t __data_size, __count, __capacity;
};

void ntg_object_map_init(ntg_object_map* ctx, size_t capacity, size_t data_size,
                         sarena* arena);

void ntg_object_map_set(ntg_object_map* ctx, const ntg_object* object,
                        void* data);

void* ntg_object_map_get(const ntg_object_map* ctx, const ntg_object* object);

#endif // NTG_OBJECT_MAP_H
