#ifndef _NTG_OBJECT_SIZES_H_
#define _NTG_OBJECT_SIZES_H_

#include <stddef.h>

typedef struct ntg_object_sizes ntg_object_sizes;
typedef struct ntg_object ntg_object;
typedef struct sarena sarena;

ntg_object_sizes* ntg_object_sizes_new(size_t child_count, sarena* arena);

void ntg_object_sizes_set(ntg_object_sizes* map, const ntg_object* object, size_t size);
size_t ntg_object_sizes_get(const ntg_object_sizes* map, const ntg_object* object);

#endif // _NTG_OBJECT_SIZES_H_
