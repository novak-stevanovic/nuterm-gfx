#ifndef NTG_OBJECT_SIZE_MAP_H
#define NTG_OBJECT_SIZE_MAP_H

#include "shared/ntg_typedef.h"

ntg_object_size_map* ntg_object_size_map_new(size_t child_count, sarena* arena);

void ntg_object_size_map_set(ntg_object_size_map* map, const ntg_object* object,
                             size_t size);

size_t ntg_object_size_map_get(const ntg_object_size_map* map,
                               const ntg_object* object);

#endif // NTG_OBJECT_SIZE_MAP_H
