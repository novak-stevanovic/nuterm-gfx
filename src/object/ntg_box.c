#include <assert.h>

#include "object/ntg_box.h"
#include "object/shared/ntg_object_vec.h"

/* -------------------------------------------------------------------------- */

static void __init_default_values(ntg_box* box)
{
    box->_orientation = NTG_ORIENTATION_HORIZONTAL;
    box->_primary_alignment = NTG_ALIGNMENT_1;
    box->_secondary_alignment = NTG_ALIGNMENT_1;
}

void __ntg_box_init__(ntg_box* box, ntg_orientation orientation,
        ntg_alignment primary_alignment, ntg_alignment secondary_alignment)
{
    assert(box != NULL);

    // __ntg_object_init__(NTG_OBJECT(box), NTG_OBJECT_WIDGET,
    //         _ntg_box_measure_fn, _ntg_box_constrain_fn,
    //         _ntg_box_arrange_children_fn, NULL);

    __init_default_values(box);
}

void __ntg_box_deinit__(ntg_box* box)
{
    assert(box != NULL);
}

void ntg_box_set_orientation(ntg_box* box, ntg_orientation orientation)
{
    assert(box != NULL);
}

void ntg_box_set_primary_alignment(ntg_box* box, ntg_alignment alignment)
{
    assert(box != NULL);
}

void ntg_box_set_secondary_alignment(ntg_box* box, ntg_alignment alignment)
{
    assert(box != NULL);
}

/* -------------------------------------------------------------------------- */
