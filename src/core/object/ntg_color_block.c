#include <assert.h>
#include <stdlib.h>

#include "core/object/ntg_color_block.h"
#include "core/scene/shared/ntg_drawing.h"
#include "core/scene/shared/ntg_measure_output.h"

#define NTG_COLOR_BLOCK_DEFAULT_SIZE 5

void __ntg_color_block_init__(ntg_color_block* color_block, nt_color color)
{
    assert(color_block != NULL);

    __ntg_object_init__(
            NTG_OBJECT(color_block),
            NTG_OBJECT_WIDGET,
            __ntg_color_block_measure_fn,
            NULL, NULL, __ntg_color_block_draw_fn,
            NULL, NULL);

    color_block->__color = color;
}

void __ntg_color_block_deinit__(ntg_color_block* color_block)
{
    assert(color_block != NULL);

    __ntg_object_deinit__(NTG_OBJECT(color_block));

    color_block->__color = NT_COLOR_DEFAULT;
}

nt_color ntg_color_block_get_color(ntg_color_block* color_block)
{
    assert(color_block != NULL);

    return color_block->__color;
}

void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color)
{
    assert(color_block != NULL);

    color_block->__color = color;
}

struct ntg_measure_output __ntg_color_block_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_context* context)
{
    return (struct ntg_measure_output) {
        .min_size = NTG_COLOR_BLOCK_DEFAULT_SIZE,
        .natural_size = NTG_COLOR_BLOCK_DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
    };
}

void __ntg_color_block_draw_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        ntg_drawing* out_drawing)
{
    const ntg_color_block* block = ntg_drawable_user(drawable);
    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = ntg_cell_bg(block->__color);
        }
    }
}
