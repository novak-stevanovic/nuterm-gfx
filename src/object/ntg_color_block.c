#include <stdlib.h>
#include "base/ntg_cell.h"
#include "object/ntg_object.h"
#include "object/shared/ntg_object_drawing.h"
#include "shared/ntg_log.h"
#include "object/ntg_color_block.h"

static void inline __update_color(ntg_color_block* block, nt_color color)
{
    block->_color = color;
    
    (NTG_SOLID_BLOCK(block))->_cell_bp = ntg_cell_bg(color);
}

void __ntg_color_block_init__(ntg_color_block* block, nt_color color)
{
    __ntg_solid_block_init__(NTG_SOLID_BLOCK(block), ntg_cell_default());

    __update_color(block, color);
}

void __ntg_color_block_deinit__(ntg_color_block* block)
{
    __ntg_solid_block_deinit__(NTG_SOLID_BLOCK(block));
}

ntg_color_block* ntg_color_block_new(nt_color color)
{
    ntg_color_block* new = (ntg_color_block*)malloc(
            sizeof(struct ntg_color_block));

    if(new == NULL) return NULL;

    __ntg_color_block_init__(new, color);

    return new;
}

void ntg_color_block_destroy(ntg_color_block* block)
{
    __ntg_color_block_deinit__(block);

    if(block != NULL)
        free(block);
}

void ntg_color_block_set_color(ntg_color_block* block, nt_color color)
{
    if(block == NULL) return;

    __update_color(block, color);
}
