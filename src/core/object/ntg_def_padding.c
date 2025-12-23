#include "core/object/ntg_def_padding.h"
#include "core/object/shared/ntg_object_drawing.h"

void _ntg_def_padding_init_(
        ntg_def_padding* def_padding,
        ntg_cell cell,
        struct ntg_padding_width init_width,
        ntg_entity_group* group,
        ntg_entity_system* system)
{
    assert(def_padding != NULL);

    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_TYPE_DEF_PADDING,
        .deinit_fn = _ntg_def_padding_deinit_fn,
        .group = group,
        .system = system
    };

    _ntg_padding_init_((ntg_padding*)def_padding, init_width,
            _ntg_def_padding_draw_fn, entity_data);

    def_padding->__cell = cell;
}

void _ntg_def_padding_deinit_fn(ntg_entity* entity)
{
    ntg_def_padding* def_padding = (ntg_def_padding*)entity;

    def_padding->__cell = ntg_cell_default();
    _ntg_padding_deinit_fn(entity);
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
