#ifndef __NTG_SIZE_MAP_H__
#define __NTG_SIZE_MAP_H__

#include <stddef.h>

typedef struct ntg_object_size_map ntg_object_size_map;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

ntg_object_size_map* ntg_object_size_map_new(size_t child_count, sarena* arena);

void ntg_object_size_map_set(ntg_object_size_map* map, const ntg_drawable* drawable, size_t size);
size_t ntg_object_size_map_get(const ntg_object_size_map* map, const ntg_drawable* drawable);

#endif // __NTG_SIZE_MAP_H__
