#ifndef _NTG_OBJECT_XY_MAP_H_
#define _NTG_OBJECT_XY_MAP_H_

#include "shared/ntg_xy.h"

typedef struct ntg_object ntg_object;

struct ntg_object_xy
{
    ntg_object* object;
    struct ntg_xy xy;
};

typedef struct ntg_object_xy_map
{
    struct ntg_object_positions* __data;
} ntg_object_xy_map;

void __ntg_object_xy_map_init__(ntg_object_xy_map* map, ntg_object* parent);
void __ntg_object_xy_map_deinit__(ntg_object_xy_map* map);

void ntg_object_xy_map_set(ntg_object_xy_map* map, ntg_object* child,
        struct ntg_xy xy);

struct ntg_xy ntg_object_xy_map_get(const ntg_object_xy_map* map,
        const ntg_object* object);

#endif // _NTG_OBJECT_XY_MAP_H_
