#ifndef _NTG_OBJECT_SIZE_MAP_H_
#define _NTG_OBJECT_SIZE_MAP_H_

#include "shared/ntg_typedef.h"

ntg_object_size_map* ntg_object_size_map_new(size_t child_count, sarena* arena);

void ntg_object_size_map_set(ntg_object_size_map* map,
        const ntg_object* object, size_t size, bool decor);
size_t ntg_object_size_map_get(const ntg_object_size_map* map,
        const ntg_object* object, bool decor);

#endif // _NTG_OBJECT_SIZE_MAP_H_
