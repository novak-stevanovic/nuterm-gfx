#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "core/object/ntg_object.h"

typedef struct ntg_box ntg_box;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

#define NTG_BOX(box_ptr) ((ntg_box*)(box_ptr))

void __ntg_box_init__(ntg_box* box, ntg_orientation orientation,
        ntg_alignment primary_alignment, ntg_alignment secondary_alignment);
void __ntg_box_deinit__(ntg_box* box);

ntg_orientation ntg_box_get_orientation(const ntg_box* box);
ntg_alignment ntg_box_get_primary_alignment(const ntg_box* box);
ntg_alignment ntg_box_get_secondary_alignment(const ntg_box* box);

void ntg_box_set_orientation(ntg_box* box, ntg_orientation orientation);
void ntg_box_set_primary_alignment(ntg_box* box, ntg_alignment alignment);
void ntg_box_set_secondary_alignment(ntg_box* box, ntg_alignment alignment);

void ntg_box_add_child(ntg_box* box, ntg_object* child);
void ntg_box_rm_child(ntg_box* box, ntg_object* child);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_box
{
    ntg_object __base;

    ntg_orientation __orientation;
    ntg_alignment __primary_alignment,
                  __secondary_alignment;
};

#endif // _NTG_BOX_H_
