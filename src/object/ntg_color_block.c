#include "base/ntg_cell.h"
#include "object/def/ntg_color_block_def.h"
#include "object/ntg_object.h"
#include <stdlib.h>
#include "object/ntg_color_block.h"

static void __measure_fn(ntg_object_t* _block)
{
    if(_block == NULL) return;

    struct ntg_xy size = _block->_adj_constr.min_size;

    ntg_object_set_size(_block, size);
}

static void __arrange_fn(ntg_object_t* _block)
{
    if(_block == NULL) return;
    ntg_color_block_t* block = (ntg_color_block_t*)_block;

    struct ntg_xy size = ntg_object_get_content_size(_block);
    size_t i, j;

    struct ntg_cell* it_cell;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(_block->_drawing, NTG_XY(j, i));

            ntg_cell_init(it_cell, NTG_CELL_EMPTY, NT_COLOR_DEFAULT,
                    block->color, NT_STYLE_DEFAULT);
        }
    }
}

static void __nsize_fn(ntg_object_t* _block)
{
    _block->_nsize = NTG_XY(0, 0);
}

void __ntg_color_block_init__(ntg_color_block_t* block)
{
    __ntg_pane_init__((ntg_pane_t*)block, __nsize_fn, __measure_fn, __arrange_fn);

    block->color = NT_COLOR_DEFAULT;
}

void __ntg_color_block_deinit__(ntg_color_block_t* block)
{
    __ntg_pane_deinit__((ntg_pane_t*)block);

    block->color = NT_COLOR_DEFAULT;
}

ntg_color_block_t* ntg_color_block_new(nt_color_t color)
{
    ntg_color_block_t* new = (ntg_color_block_t*)malloc(
            sizeof(struct ntg_color_block));

    if(new == NULL) return NULL;

    __ntg_color_block_init__(new);
    new->color = color;

    return new;
}

void ntg_color_block_destroy(ntg_color_block_t* block)
{
    __ntg_color_block_deinit__(block);

    free(block);
}

void ntg_color_block_set_color(ntg_color_block_t* block, nt_color_t color)
{
    if(block == NULL) return;

    block->color = color;
}

nt_color_t ntg_color_block_get_color(ntg_color_block_t* block)
{
    return (block != NULL) ? block->color : NT_COLOR_DEFAULT;
}
