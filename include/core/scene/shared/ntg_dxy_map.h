#ifndef __NTG_DXY_MAP_H__
#define __NTG_DXY_MAP_H__

#include <stddef.h>
#include "shared/ntg_xy.h"

typedef struct ntg_dxy_map ntg_dxy_map;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

ntg_dxy_map* ntg_dxy_map_new(size_t child_count, sarena* arena);

void ntg_dxy_map_set(ntg_dxy_map* map,
        const ntg_drawable* drawable,
        struct ntg_xy xy);
struct ntg_xy ntg_dxy_map_get(
        const ntg_dxy_map* map,
        const ntg_drawable* drawable);

#endif // __NTG_DXY_MAP_H__
