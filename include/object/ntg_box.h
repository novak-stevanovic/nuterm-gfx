#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "object/ntg_container.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

typedef enum ntg_box_orientation
{ 
    NTG_BOX_ORIENTATION_HORIZONTAL,
    NTG_BOX_ORIENTATION_VERTICAL 
} ntg_box_orientation;

typedef enum ntg_box_alignment
{
    /* left, when used in reference to x axis alignment */
    /* top, when used in reference to y axis alignment */
    NTG_BOX_ALIGNMENT_0,

    /* center */
    NTG_BOX_ALIGNMENT_1,

    /* right, when used in reference to x axis alignment */
    /* bottom, when used in reference to y axis alignment */
    NTG_BOX_ALIGNMENT_2

} ntg_box_alignment;

struct ntg_box_padding
{
    size_t north, east, south, west;
};

static const struct ntg_box_padding NTG_BOX_PADDING_UNSET = {0};

typedef struct ntg_box ntg_box;

void __ntg_box_init__(ntg_box* box,
        ntg_box_orientation orientation,
        ntg_box_alignment primary_alignment,
        ntg_box_alignment secondary_alignment);

void __ntg_box_deinit__(ntg_box* box);

ntg_box* ntg_box_new(ntg_box_orientation orientation,
        ntg_box_alignment primary_alignment,
        ntg_box_alignment secondary_alignment);

void ntg_box_destroy(ntg_box* box);

void ntg_box_add_child(ntg_box* box, ntg_object* object);
void ntg_box_remove_child(ntg_box* box, ntg_object* object);

void ntg_box_set_padding(ntg_box* box, struct ntg_box_padding padding);
void ntg_box_set_orientation(ntg_box* box, ntg_box_orientation orientation);
void ntg_box_set_primary_alignment(ntg_box* box, ntg_box_alignment alignment);
void ntg_box_set_secondary_alignment(ntg_box* box, ntg_box_alignment alignment);

void ntg_box_set_bg(ntg_box* box, ntg_object* bg);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

// TODO
#define NTG_BOX_MAX_CHILDREN 50

struct ntg_box
{
    ntg_container __base;

    ntg_box_orientation _orientation;
    ntg_box_alignment _primary_alignment, _secondary_alignment;

    struct ntg_box_padding _pref_padding;

    /* Cached - initialized in nsize phase -------------- */

    struct ntg_xy __content_nsize; // children combined natural size

    /* Cached - initialized in constrain phase ---------- */

    struct ntg_box_padding __padding; // applied padding
    struct ntg_constr __content_constr; // content constraints(children)

    /* Cached - initialized in measure phase ------------ */

    struct ntg_xy __content_size; // size of rectangle containing children
    struct ntg_xy  __content_box_size; // size - padding_size
};

#endif // _NTG_BOX_H
