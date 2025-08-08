#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "object/ntg_object.h"

typedef struct ntg_box ntg_box;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

#define NTG_BOX(box_ptr) ((ntg_box*)(box_ptr))

void __ntg_box_init__(ntg_box* box, ntg_orientation orientation,
        ntg_alignment primary_alignment, ntg_alignment secondary_alignment);
void __ntg_box_deinit__(ntg_box* box);

void ntg_box_set_orientation(ntg_box* box, ntg_orientation orientation);
void ntg_box_set_primary_alignment(ntg_box* box, ntg_alignment alignment);
void ntg_box_set_secondary_alignment(ntg_box* box, ntg_alignment alignment);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_box
{
    ntg_object __base;

    ntg_orientation _orientation;
    ntg_alignment _primary_alignment,
                  _secondary_alignment;
};

struct ntg_measure_object _ntg_box_measure_fn(ntg_object* _box,
        ntg_orientation orientation, size_t for_size);
void _ntg_box_constrain_fn(ntg_object* _box, ntg_orientation orientation,
        size_t size, ntg_object_size_map* out_sizes);
void _ntg_box_arrange_children_fn(ntg_object* _box, struct ntg_xy size,
        ntg_object_xy_map* out_positions);

#endif // _NTG_BOX_H_
