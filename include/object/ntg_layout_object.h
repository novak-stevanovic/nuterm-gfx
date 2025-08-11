#ifndef _NTG_LAYOUT_OBJECT_H_
#define _NTG_LAYOUT_OBJECT_H_

#include "object/ntg_object.h"

typedef struct ntg_layout_object
{
    ntg_object* _object;
    struct ntg_xy _size;
    ntg_orientation _orientation;

    SArena* __arena;
} ntg_layout_object;

void __ntg_layout_object_init__(ntg_layout_object* layout_object,
       ntg_object* object, struct ntg_xy size, ntg_orientation orientation);
void __ntg_layout_object_deinit__(ntg_layout_object* layout_object);
void ntg_layout_object_perform(ntg_layout_object* layout_object);

#endif // _NTG_LAYOUT_OBJECT_H_
