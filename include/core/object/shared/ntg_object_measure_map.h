#ifndef _NTG_OBJECT_MEASURE_MAP_H_
#define _NTG_OBJECT_MEASURE_MAP_H_

#include "shared/ntg_typedef.h"

ntg_object_measure_map* ntg_object_measure_map_new(size_t child_count, sarena* arena);

void ntg_object_measure_map_set(ntg_object_measure_map* map,
        const ntg_object* object,
        struct ntg_object_measure measure, bool decor);
struct ntg_object_measure ntg_object_measure_map_get(
        const ntg_object_measure_map* map,
        const ntg_object* object, bool decor);

#endif // _NTG_OBJECT_MEASURE_MAP_H_
