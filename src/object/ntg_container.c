#include "base/ntg_cell.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "object/ntg_object.h"
#include <assert.h>
#include "object/ntg_container.h"

void __ntg_container_init__(ntg_container* container,
        ntg_nsize_fn nsize_fn, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_fn arrange_fn)
        
{
    ntg_object* _container = (ntg_object*)container;

    __ntg_object_init__(_container, nsize_fn, constrain_fn, measure_fn,
            arrange_fn);

    container->_bg = NULL;
    _container->_children = ntg_object_vec_new();
}

void __ntg_container_deinit__(ntg_container* container)
{
    __ntg_object_deinit__((ntg_object*)container);
}

void _ntg_container_arrange_children(ntg_container* container)
{
    if(container == NULL) return;
}

void _ntg_container_set_bg(ntg_container* container, ntg_object* bg)
{
    assert(container != NULL);

    if(container->_bg != NULL)
        _ntg_object_children_remove(NTG_OBJECT(container), bg);

    if(bg != NULL)
    {
        struct ntg_aux_object aux_bg = {
            .object = bg,
            .z_index = 0
        };

        _ntg_object_children_add_aux(NTG_OBJECT(container), aux_bg);
    }

    container->_bg = bg;
}
