#ifndef _NTG_OBJECT_XYS_H_
#define _NTG_OBJECT_XYS_H_

#include <stddef.h>
#include "shared/ntg_xy.h"

typedef struct ntg_object_xys ntg_object_xys;
typedef struct ntg_object ntg_object;
typedef struct sarena sarena;

ntg_object_xys* ntg_object_xys_new(size_t child_count, sarena* arena);

void ntg_object_xys_set(ntg_object_xys* map,
        const ntg_object* object,
        struct ntg_xy xy);
struct ntg_xy ntg_object_xys_get(
        const ntg_object_xys* map,
        const ntg_object* object);

#endif // _NTG_OBJECT_XYS_H_
