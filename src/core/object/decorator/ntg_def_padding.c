#include "ntg.h"
#include <assert.h>

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_padding* ntg_def_padding_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_DEF_PADDING,
        .deinit_fn = (ntg_entity_deinit_fn)ntg_def_padding_deinit,
        .system = system
    };
    ntg_def_padding* new = (ntg_def_padding*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_def_padding_init(ntg_def_padding* def_padding)
{
    assert(def_padding != NULL);

    ntg_decorator_init((ntg_decorator*)def_padding, _ntg_def_padding_draw_fn);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_def_padding_deinit(ntg_def_padding* padding)
{
    ntg_decorator_deinit((ntg_decorator*)padding);
}

void _ntg_def_padding_draw_fn(
        const ntg_object* _padding,
        void* _ldata,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena)
{
    struct ntg_vcell cell;
    struct ntg_xy size = _padding->_size;

    if(_padding->_children.size == 0)
    {
        cell = ntg_vcell_default();
    }
    else
    {
        const ntg_object* child = _padding->_children.data[0];
        cell = ((const ntg_decorator*)_padding)->_widget->_background;
    }

    size_t i, j;
    struct ntg_vcell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_tmp_object_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = cell;
        }
    }
}
