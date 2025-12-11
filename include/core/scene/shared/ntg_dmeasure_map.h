#ifndef __NTG_DMEASURE_MAP_H__
#define __NTG_DMEASURE_MAP_H__

#include <stddef.h>
#include "core/scene/shared/ntg_measurement.h"

typedef struct ntg_dmeasure_map ntg_dmeasure_map;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

ntg_dmeasure_map* ntg_dmeasure_map_new(size_t child_count, sarena* arena);

void ntg_dmeasure_map_set(ntg_dmeasure_map* map,
        const ntg_drawable* drawable,
        struct ntg_measurement measurement);
struct ntg_measurement ntg_dmeasure_map_get(
        const ntg_dmeasure_map* map,
        const ntg_drawable* drawable);

#endif // __NTG_DMEASURE_MAP_H__
