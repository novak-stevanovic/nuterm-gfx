#include "base/ntg_cell.h"
#include "object/def/ntg_container_def.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "object/ntg_object.h"
#include "object/ntg_container.h"

static void __arrange_fn(ntg_object_t* object)
{
    _ntg_container_arrange_children((ntg_container_t*)object);

    ntg_container_t* container = (ntg_container_t*)object;

    ntg_object_drawing_t* drawing = _ntg_object_get_drawing_(object);
    struct ntg_xy size = ntg_object_drawing_get_size(drawing);

    size_t i, j;
    ntg_cell_t* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(drawing, ntg_xy(j, i));

            (*it_cell) = ntg_cell_full(NTG_CELL_EMPTY,
                    NT_COLOR_DEFAULT, container->_bg, NT_STYLE_DEFAULT);
        }
    }
}

void __ntg_container_init__(ntg_container_t* container,
        ntg_nsize_fn nsize_fn, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_children_fn arrange_fn)
        
{
    ntg_object_t* _container = (ntg_object_t*)container;

    container->_bg = NT_COLOR_DEFAULT;
    container->__arrange_children_fn = arrange_fn;

    __ntg_object_init__(_container, nsize_fn, constrain_fn, measure_fn,
            __arrange_fn);
}

void __ntg_container_deinit__(ntg_container_t* container)
{
    container->_bg = NT_COLOR_DEFAULT;

    __ntg_object_deinit__((ntg_object_t*)container);
}

void _ntg_container_arrange_children(ntg_container_t* container)
{
    if(container == NULL) return;

    if(container->__arrange_children_fn != NULL)
    {
        container->__arrange_children_fn(container);
    }
}

void _ntg_container_add_child(ntg_container_t* container, ntg_object_t* object)
{
    if(container == NULL) return;

    ntg_object_t* _container = (ntg_object_t*)container;

    ntg_object_vec_append(_container->_children, object);

    object->_parent = _container;
}

void _ntg_container_remove_child(ntg_container_t* container, ntg_object_t* object)
{
    if(container == NULL) return;

    ntg_object_t* _container = (ntg_object_t*)container;

    ntg_object_vec_remove(_container->_children, object);

    object->_parent = NULL;
}

void _ntg_container_set_bg(ntg_container_t* container, nt_color_t bg)
{
    if(container == NULL) return;

    container->_bg = bg;
}
