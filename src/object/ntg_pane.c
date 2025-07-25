#include "object/ntg_pane.h"

void __ntg_pane_init__(ntg_pane* pane, ntg_natural_size_fn calculate_nsize_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn)
{
    __ntg_object_init__((ntg_object*)pane, calculate_nsize_fn, NULL,
            measure_fn, arrange_fn);
}

void __ntg_pane_deinit__(ntg_pane* pane)
{
    __ntg_object_deinit__((ntg_object*)pane);
}
