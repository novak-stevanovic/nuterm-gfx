#include "object/ntg_pane.h"
#include "object/shared/ntg_object_drawing.h"

void __ntg_pane_init__(ntg_pane* pane, ntg_nsize_fn nsize_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn)
{
    ntg_object* _pane = NTG_OBJECT(pane);

    __ntg_object_init__(_pane, nsize_fn, NULL,
            measure_fn, arrange_fn);

    _pane->_drawing = ntg_object_drawing_new();
}

void __ntg_pane_deinit__(ntg_pane* pane)
{
    __ntg_object_deinit__((ntg_object*)pane);
}
