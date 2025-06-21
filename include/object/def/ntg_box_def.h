#ifndef _NTG_BOX_DEF_H_
#define _NTG_BOX_DEF_H_

#include "object/ntg_box.h"

#define __NTG_ALLOW_CONTAINER_DEF__
#include "object/def/ntg_container_def.h"
#undef __NTG_ALLOW_CONTAINER_DEF__

#define NTG_BOX_MAX_CHILDREN 50

struct ntg_box
{
    ntg_container_t _base;

    ntg_box_orientation_t _orientation;
    ntg_box_alignment_t _primary_alignment, _secondary_alignment;

    struct ntg_box_padding _pref_padding;
    size_t _spacing;

    /* Cached - initialized in nsize phase -------------- */

    /* Children natural size */
    struct ntg_xy _children_nsize;

    /* Cached - initialized in constrain phase ---------- */

    struct ntg_box_padding _padding;

    /* Content constraints */
    struct ntg_constr _content_constr;

    /* Cached - initialized in measure phase ------------ */

    /* Size of rectangle containing children */
    struct ntg_xy _content_size;
    /* Size - padding_size */
    struct ntg_xy _content_box_size;
};

void __ntg_box_init__(ntg_box_t* box,
        ntg_box_orientation_t orientation,
        ntg_box_alignment_t primary_alignment,
        ntg_box_alignment_t secondary_alignment);
void __ntg_box_deinit__(ntg_box_t* box);

void _ntg_box_on_nsize(ntg_box_t* box);
void _ntg_box_on_constrain(ntg_box_t* box);

#endif // _NTG_BOX_DEF_H_
