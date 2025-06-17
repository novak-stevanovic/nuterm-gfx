#include <stdlib.h>
#include "base/ntg_cell.h"
#include "object/def/ntg_color_block_def.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/ntg_log.h"
#include "object/ntg_color_block.h"

static void __measure_fn(ntg_object_t* _block)
{
    if(_block == NULL) return;

    struct ntg_xy size = ntg_object_get_constr(_block).min_size;

    ntg_log_log("%p %d %d", _block, size.x, size.y);

    ntg_xy_size(&size);

    _ntg_object_set_size(_block, size);
}

static void __arrange_fn(ntg_object_t* _block)
{
    if(_block == NULL) return;
    ntg_color_block_t* block = (ntg_color_block_t*)_block;

    ntg_object_drawing_t* drawing = _ntg_object_get_drawing_(_block);
    struct ntg_xy size = ntg_object_drawing_get_size(drawing);
    ntg_xy_size(&size);
    size_t i, j;

    struct ntg_cell* it_cell;

    nt_color_t it_color;
    int it_letter;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_letter = ((i + j) % 26) + 'a';

            it_cell = ntg_object_drawing_at_(drawing, ntg_xy(j, i));

            *it_cell = ntg_cell_full(it_letter, nt_color_new(0, 0, 0),
                    block->__color, NT_STYLE_DEFAULT);
        }
    }
}

static void __nsize_fn(ntg_object_t* _block)
{
    struct ntg_xy size = ntg_object_get_pref_size(_block);
    ntg_xy_size(&size);
    _ntg_object_set_nsize(_block, size);
}

void __ntg_color_block_init__(ntg_color_block_t* block)
{
    __ntg_pane_init__((ntg_pane_t*)block, __nsize_fn, __measure_fn, __arrange_fn);

    block->__color = NT_COLOR_DEFAULT;
}

void __ntg_color_block_deinit__(ntg_color_block_t* block)
{
    __ntg_pane_deinit__((ntg_pane_t*)block);

    block->__color = NT_COLOR_DEFAULT;
}

ntg_color_block_t* ntg_color_block_new(nt_color_t color)
{
    ntg_color_block_t* new = (ntg_color_block_t*)malloc(
            sizeof(struct ntg_color_block));

    if(new == NULL) return NULL;

    __ntg_color_block_init__(new);
    new->__color = color;

    return new;
}

void ntg_color_block_destroy(ntg_color_block_t* block)
{
    __ntg_color_block_deinit__(block);

    if(block != NULL)
        free(block);
}

void ntg_color_block_set_color(ntg_color_block_t* block, nt_color_t color)
{
    if(block == NULL) return;

    block->__color = color;
}

nt_color_t ntg_color_block_get_color(ntg_color_block_t* block)
{
    return (block != NULL) ? block->__color : NT_COLOR_DEFAULT;
}
