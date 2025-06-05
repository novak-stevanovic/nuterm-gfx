#include "object/def/ntg_pane_def.h"

void __ntg_pane_init__(ntg_pane_t* pane, ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn)
{
    __ntg_object_init__((ntg_object_t*)pane, NULL, measure_fn, arrange_fn);
}

void __ntg_pane_deinit__(ntg_pane_t* pane)
{
    __ntg_object_deinit__((ntg_object_t*)pane);
}
