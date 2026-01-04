#include "ntg.h"
#include <assert.h>

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_padding* ntg_def_padding_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_DEF_PADDING,
        .deinit_fn = _ntg_def_padding_deinit_fn,
        .system = system
    };
    ntg_def_padding* new = (ntg_def_padding*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_def_padding_init(ntg_def_padding* def_padding)
{
    assert(def_padding != NULL);

    ntg_padding_init((ntg_padding*)def_padding, _ntg_def_padding_draw_fn);
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_padding_deinit_fn(ntg_entity* entity)
{
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
    struct ntg_vcell cell;

    if(ntg_object_get_children(object).count == 0)
    {
        cell = ntg_vcell_default();
    }
    else
    {
        const ntg_object* child = ntg_object_get_children(object).data[0];
        cell = child->_background;
    }

    size_t i, j;
    struct ntg_vcell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(out->drawing, ntg_xy(j, i));
            (*it_cell) = cell;
        }
    }
}
