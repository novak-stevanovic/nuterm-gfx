#ifndef __NTG_OMEASURE_MAP_H__
#define __NTG_OMEASURE_MAP_H__

#include <stddef.h>
#include "core/object/shared/ntg_object_measure.h"

typedef struct ntg_omeasure_map ntg_omeasure_map;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

ntg_omeasure_map* ntg_omeasure_map_new(size_t child_count, sarena* arena);

void ntg_omeasure_map_set(ntg_omeasure_map* map,
        const ntg_drawable* drawable,
        struct ntg_object_measure measure);
struct ntg_object_measure ntg_omeasure_map_get(
        const ntg_omeasure_map* map,
        const ntg_drawable* drawable);

#endif // __NTG_OMEASURE_MAP_H__
