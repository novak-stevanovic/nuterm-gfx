#ifndef _NTG_OBJECT_MEASURE_MAP_H_
#define _NTG_OBJECT_MEASURE_MAP_H_

#include <stddef.h>
#include "core/object/shared/ntg_object_measure.h"

typedef struct ntg_object_measure_map ntg_object_measure_map;
typedef struct ntg_object ntg_object;
typedef struct sarena sarena;

ntg_object_measure_map* ntg_object_measure_map_new(size_t child_count, sarena* arena);

void ntg_object_measure_map_set(ntg_object_measure_map* map,
        const ntg_object* object,
        struct ntg_object_measure measure);
struct ntg_object_measure ntg_object_measure_map_get(
        const ntg_object_measure_map* map,
        const ntg_object* object);

#endif // _NTG_OBJECT_MEASURE_MAP_H_
