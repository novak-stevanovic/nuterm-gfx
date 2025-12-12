#ifndef _NTG_OBJECT_SIZE_MAP_H_
#define _NTG_OBJECT_SIZE_MAP_H_

#include <stddef.h>

typedef struct ntg_object_size_map ntg_object_size_map;
typedef struct ntg_object ntg_object;
typedef struct sarena sarena;

ntg_object_size_map* ntg_object_size_map_new(size_t child_count, sarena* arena);

void ntg_object_size_map_set(ntg_object_size_map* map,
        const ntg_object* object, size_t size);
size_t ntg_object_size_map_get(const ntg_object_size_map* map,
        const ntg_object* object);

#endif // _NTG_OBJECT_SIZE_MAP_H_
