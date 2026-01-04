#ifndef _NTG_OBJECT_XY_MAP_H_
#define _NTG_OBJECT_XY_MAP_H_

#include "shared/ntg_typedef.h"

ntg_object_xy_map* ntg_object_xy_map_new(size_t child_count, sarena* arena);

void ntg_object_xy_map_set(ntg_object_xy_map* map,
        const ntg_object* object,
        struct ntg_xy xy);
struct ntg_xy ntg_object_xy_map_get(
        const ntg_object_xy_map* map,
        const ntg_object* object);

#endif // _NTG_OBJECT_XY_MAP_H_
