#include "ntg.h"
#include <assert.h>
#define DEFAULT_SIZE 1

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color)
{
    assert(color_block != NULL);

    color_block->__color = color;

    _ntg_object_mark_change((ntg_object*)color_block);
}

ntg_color_block* ntg_color_block_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_COLOR_BLOCK,
        .deinit_fn = _ntg_color_block_deinit_fn,
        .system = system
    };

    ntg_color_block* new = (ntg_color_block*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void ntg_color_block_init(ntg_color_block* color_block)
{
    assert(color_block != NULL);

    struct ntg_object_layout_ops object_data = {
        .measure_fn = _ntg_color_block_measure_fn,
        .constrain_fn = NULL,
        .arrange_fn = NULL,
        .draw_fn = _ntg_color_block_draw_fn,
    };

    _ntg_object_init((ntg_object*)color_block, object_data);

    color_block->__color = NT_COLOR_DEFAULT;
}


/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_color_block_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_color_block* color_block = (ntg_color_block*)entity;

    color_block->__color = NT_COLOR_DEFAULT;
    _ntg_object_deinit_fn(entity);
}

struct ntg_object_measure _ntg_color_block_measure_fn(
        const ntg_object* _block,
        void* _layout_data,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena)
{
    return (struct ntg_object_measure) {
        .min_size = DEFAULT_SIZE,
        .natural_size = DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
        .grow = 1
    };
}

void _ntg_color_block_draw_fn(
        const ntg_object* _block,
        void* _layout_data,
        struct ntg_xy size,
        ntg_temp_object_drawing* out_drawing,
        sarena* arena)
{
    ntg_color_block* color_block = (ntg_color_block*)_block;

    size_t i, j;
    struct ntg_vcell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_temp_object_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = ntg_vcell_bg(color_block->__color);
        }
    }
}
