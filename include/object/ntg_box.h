#ifndef _NTG_BOX_H_
#define _NTG_BOX_H_

#include "object/ntg_container.h"
#include "object/ntg_padding.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

typedef struct ntg_box ntg_box;

void __ntg_box_init__(ntg_box* box,
        ntg_orientation orientation,
        ntg_alignment primary_alignment,
        ntg_alignment secondary_alignment);

void __ntg_box_deinit__(ntg_box* box);

ntg_box* ntg_box_new(ntg_orientation orientation,
        ntg_alignment primary_alignment,
        ntg_alignment secondary_alignment);

void ntg_box_destroy(ntg_box* box);

void ntg_box_add_child(ntg_box* box, ntg_object* object);
void ntg_box_remove_child(ntg_box* box, ntg_object* object);

void ntg_box_set_padding_size(ntg_box* box, struct ntg_padding_size padding);
void ntg_box_set_orientation(ntg_box* box, ntg_orientation orientation);
void ntg_box_set_primary_alignment(ntg_box* box, ntg_alignment alignment);
void ntg_box_set_secondary_alignment(ntg_box* box, ntg_alignment alignment);

void ntg_box_set_bg_color(ntg_box* box, nt_color color);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

// TODO
#define NTG_BOX_MAX_CHILDREN 50

struct ntg_box
{
    ntg_container __base;

    ntg_orientation _orientation;
    ntg_alignment _primary_alignment, _secondary_alignment;

    struct ntg_padding_size _padding;

    /* Content - object's content without the object borders
     * Box content - children
     * Box content extra - children + "extra" padding */

    /* Cached - initialized in nsize phase -------------- */

    struct ntg_xy __box_content_nsize;

    /* Cached - initialized in constrain phase ---------- */

    struct ntg_padding_size __applied_padding; // applied padding
    struct ntg_constr __box_content_constr;

    /* Cached - initialized in measure phase ------------ */

    struct ntg_xy __box_content_size;
    struct ntg_xy  __box_content_extra_size;
};

#endif // _NTG_BOX_H
