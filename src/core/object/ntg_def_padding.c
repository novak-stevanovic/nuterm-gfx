#include "core/object/ntg_def_padding.h"
#include "core/object/shared/ntg_object_drawing.h"

void _ntg_def_padding_init_(
        ntg_def_padding* def_padding,
        ntg_cell cell,
        struct ntg_padding_width init_width,
        ntg_object_container* container)
{
    assert(def_padding != NULL);

    _ntg_padding_init_((ntg_padding*)def_padding,
            NTG_PADDING_PADDING,
            init_width,
            _ntg_def_padding_draw_fn,
            _ntg_def_padding_deinit_fn, NULL,
            container);

    def_padding->__cell = cell;
}

void _ntg_def_padding_deinit_fn(ntg_object* object)
{
    ntg_def_padding* def_padding = (ntg_def_padding*)object;

    _ntg_padding_deinit_fn(object);

    def_padding->__cell = ntg_cell_default();
}

void _ntg_def_padding_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* _layout_data,
        sarena* arena)
{
    ntg_def_padding* def_padding = (ntg_def_padding*)object;

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(out->drawing, ntg_xy(j, i));
            (*it_cell) = def_padding->__cell;
        }
    }
}
