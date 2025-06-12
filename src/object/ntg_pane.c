#include "object/def/ntg_pane_def.h"
#include "object/shared/ntg_object_drawing.h"

void __ntg_pane_init__(ntg_pane_t* pane, ntg_nsize_fn nsize_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn)
{
    ntg_object_drawing_t* drawing = ntg_object_drawing_new();

    __ntg_object_init__((ntg_object_t*)pane, nsize_fn, NULL,
            measure_fn, arrange_fn, NULL, drawing);
}

void __ntg_pane_deinit__(ntg_pane_t* pane)
{
    __ntg_object_deinit__((ntg_object_t*)pane);
}
