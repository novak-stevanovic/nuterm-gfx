#ifndef _NTG_POSITIONS_OBJECT_H_
#define _NTG_POSITIONS_OBJECT_H_

#include "shared/ntg_xy.h"

typedef struct ntg_object ntg_object;

struct ntg_object_positions
{
    ntg_object* object;
    struct ntg_xy pos;
};

typedef struct ntg_positions_object
{
    struct ntg_object_positions* __data;
} ntg_positions_object;

void __ntg_positions_object_init__(ntg_positions_object* positions_object, ntg_object* parent);
void __ntg_positions_object_deinit__(ntg_positions_object* positions_object);

void ntg_positions_object_set(ntg_positions_object* positions_object,
        ntg_object* child, struct ntg_xy pos);

#endif // _NTG_POSITIONS_OBJECT_H_
