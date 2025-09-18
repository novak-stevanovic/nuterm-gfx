#include <assert.h>

#include "object/ntg_color_block.h"
#include "object/shared/ntg_object_drawing.h"

#define NTG_COLOR_BLOCK_DEFAULT_SIZE 5

void __ntg_color_block_init__(ntg_color_block* color_block, nt_color color)
{
    assert(color_block != NULL);

    __ntg_object_init__(
            NTG_OBJECT(color_block), NTG_OBJECT_WIDGET,
            _ntg_color_block_measure_fn, NULL, NULL,
            _ntg_color_block_arrange_drawing_fn);

    color_block->__color = color;
}

void __ntg_color_block_deinit__(ntg_color_block* color_block)
{
    assert(color_block != NULL);

    __ntg_object_deinit__(NTG_OBJECT(color_block));

    color_block->__color = NT_COLOR_DEFAULT;
}

ntg_color_block* ntg_color_block_new(nt_color color)
{
    ntg_color_block* new = (ntg_color_block*)malloc(sizeof(ntg_color_block));
    assert(new != NULL);

    __ntg_color_block_init__(new, color);

    return new;
}

void ntg_color_block_destroy(ntg_color_block* color_block)
{
    assert(color_block != NULL);

    __ntg_color_block_deinit__(color_block);

    free(color_block);
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

struct ntg_measure_result _ntg_color_block_measure_fn(const ntg_object* _block,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context)
{
    return (struct ntg_measure_result) {
        .min_size = NTG_COLOR_BLOCK_DEFAULT_SIZE,
        .natural_size = NTG_COLOR_BLOCK_DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
    };
}

void _ntg_color_block_arrange_drawing_fn(const ntg_object* _block,
        struct ntg_xy size, ntg_object_drawing* out_drawing)
{
    ntg_color_block* block = NTG_CBLOCK(_block);

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(out_drawing, ntg_xy(j, i));
            (*it_cell) = ntg_cell_bg(block->__color);
        }
    }
}
