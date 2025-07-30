#include "base/ntg_cell.h"
#include "base/ntg_event_types.h"
#include "object/shared/ntg_object_drawing.h"
#include "object/shared/ntg_object_vec.h"
#include "object/ntg_object.h"
#include "object/ntg_container.h"

static void __arrange_fn(ntg_object* object)
{
    _ntg_container_arrange_children((ntg_container*)object);

    ntg_container* container = (ntg_container*)object;

    ntg_object_drawing* drawing = object->_virtual_drawing;
    struct ntg_xy size = ntg_object_drawing_get_size(drawing);

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(drawing, ntg_xy(j, i));

            (*it_cell) = ntg_cell_full(NTG_CELL_EMPTY,
                    nt_color_new(255, 255, 255), container->__bg, NT_STYLE_DEFAULT);
        }
    }
}

void __ntg_container_init__(ntg_container* container,
        ntg_natural_size_fn calculate_nsize_fn, ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn, ntg_arrange_children_fn arrange_fn)
{
    ntg_object* _container = (ntg_object*)container;

    container->__bg = NT_COLOR_DEFAULT;
    container->__arrange_children_fn = arrange_fn;

    __ntg_object_init__(_container, calculate_nsize_fn, constrain_fn, measure_fn,
            __arrange_fn);
}

void __ntg_container_deinit__(ntg_container* container)
{
    container->__bg = NT_COLOR_DEFAULT;

    __ntg_object_deinit__((ntg_object*)container);
}

void _ntg_container_arrange_children(ntg_container* container)
{
    if(container == NULL) return;

    if(container->__arrange_children_fn != NULL)
    {
        container->__arrange_children_fn(container);
    }
}

void _ntg_container_set_bg(ntg_container* container, nt_color bg)
{
    if(container == NULL) return;

    if(nt_color_cmp(container->__bg, bg)) return;

    container->__bg = bg;

    ntg_listenable* listenable = _ntg_object_get_listenable(NTG_OBJECT(container));
    ntg_event e1;
    __ntg_event_init__(&e1, NTG_ETYPE_OBJECT_CONTENT_INVALID, container, NULL);
    ntg_listenable_raise(listenable, &e1);
}
