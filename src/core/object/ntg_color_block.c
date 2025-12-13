#include <assert.h>
#include <stdlib.h>

#include "core/object/ntg_color_block.h"
#include "core/object/shared/ntg_object_drawing.h"
#include "core/object/shared/ntg_object_types.h"
#include "core/object/shared/ntg_object_measure.h"
#include "shared/_ntg_shared.h"

#define NTG_COLOR_BLOCK_DEFAULT_SIZE 5

void _ntg_color_block_init_(
        ntg_color_block* color_block,
        nt_color color,
        ntg_object_process_key_fn process_key_fn,
        ntg_object_focus_fn on_focus_fn,
        ntg_object_unfocus_fn on_unfocus_fn,
        ntg_object_deinit_fn deinit_fn,
        void* data)
{
    assert(color_block != NULL);

    _ntg_object_init_(
            (ntg_object*)color_block,
            NTG_OBJECT_COLOR_BLOCK,
            _ntg_color_block_measure_fn,
            NULL,
            NULL,
            _ntg_color_block_draw_fn,
            process_key_fn,
            on_focus_fn,
            on_unfocus_fn,
            (deinit_fn != NULL) ? deinit_fn : _ntg_color_block_deinit_fn,
            data);


    color_block->__color = color;
}

void _ntg_color_block_deinit_(ntg_color_block* color_block)
{
    assert(color_block != NULL);

    _ntg_color_block_deinit_fn((ntg_object*)color_block);
}

nt_color ntg_color_block_get_color(const ntg_color_block* color_block)
{
    assert(color_block != NULL);

    return color_block->__color;
}

void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color)
{
    assert(color_block != NULL);

    color_block->__color = color;
}

void _ntg_color_block_deinit_fn(ntg_object* object)
{
    assert(object != NULL);

    ntg_color_block* color_block = (ntg_color_block*)object;

    _ntg_object_deinit_(object);

    color_block->__color = NT_COLOR_DEFAULT;
}

struct ntg_object_measure _ntg_color_block_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena)
{
    return (struct ntg_object_measure) {
        .min_size = NTG_COLOR_BLOCK_DEFAULT_SIZE,
        .natural_size = NTG_COLOR_BLOCK_DEFAULT_SIZE,
        .max_size = NTG_SIZE_MAX,
    };
}

void _ntg_color_block_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena)
{
    ntg_color_block* color_block = (ntg_color_block*)object;

    size_t i, j;
    ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(out->drawing, ntg_xy(j, i));
            (*it_cell) = ntg_cell_bg(color_block->__color);
        }
    }
}
