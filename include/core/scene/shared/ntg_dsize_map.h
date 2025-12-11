#ifndef __NTG_DSIZE_MAP_H__
#define __NTG_DSIZE_MAP_H__

#include <stddef.h>

typedef struct ntg_dsize_map ntg_dsize_map;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

ntg_dsize_map* ntg_dsize_map_new(size_t child_count, sarena* arena);

void ntg_dsize_map_set(ntg_dsize_map* map, const ntg_drawable* drawable, size_t size);
size_t ntg_dsize_map_get(const ntg_dsize_map* map, const ntg_drawable* drawable);

#endif // __NTG_DSIZE_MAP_H__
