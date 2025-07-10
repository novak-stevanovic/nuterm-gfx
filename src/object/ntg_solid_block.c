#include "object/ntg_solid_block.h"
#include "object/shared/ntg_object_drawing.h"
#include <assert.h>
#include <stdlib.h>

static void __calculate_nsize_fn(ntg_object* _solid_block)
{
    _ntg_object_set_content_nsize(_solid_block,
            ntg_xy_size(_solid_block->_pref_size));
}

static void __measure_fn(ntg_object* _solid_block)
{
    _ntg_object_set_content_size(_solid_block,
            ntg_object_get_content_constr(_solid_block).min_size);
}

static void __arrange_fn(ntg_object* _solid_block)
{
    ntg_object_drawing* drawing = _solid_block->_virtual_drawing;
    struct ntg_xy size = ntg_object_drawing_get_size(drawing);

    size_t i, j;
    struct ntg_cell* it_cell;
    char it_letter;
    for(i = 0; i < size.y; i++)
    {
        for(j = 0; j < size.x; j++)
        {
            it_cell = ntg_object_drawing_at_(drawing, ntg_xy(j, i));

            // TODO:

            (*it_cell) = (NTG_SOLID_BLOCK(_solid_block))->_cell_bp;

            it_letter = 'a' + ((i + j) % 26);
            it_cell->__base.codepoint = it_letter;
        }
    }
}

void __ntg_solid_block_init__(ntg_solid_block* solid_block, ntg_cell cell_bp)
{
    assert(solid_block != NULL);

    __ntg_pane_init__(NTG_PANE(solid_block),
            __calculate_nsize_fn,
            __measure_fn,
            __arrange_fn);

    solid_block->_cell_bp = cell_bp;
}

void __ntg_solid_block_deinit__(ntg_solid_block* solid_block)
{
    assert(solid_block != NULL);

    __ntg_pane_deinit__(NTG_PANE(solid_block));

    solid_block->_cell_bp = ntg_cell_default();
}

ntg_solid_block* ntg_solid_block_new(ntg_cell cell_bp)
{
    ntg_solid_block* new = (ntg_solid_block*)malloc(sizeof(struct ntg_solid_block));

    assert(new != NULL);

    __ntg_solid_block_init__(new, cell_bp);

    return new;
}

void ntg_solid_block_destroy(ntg_solid_block* solid_block)
{
    assert(solid_block != NULL);

    __ntg_solid_block_deinit__(solid_block);

    free(solid_block);
}

void ntg_solid_block_set_cell_bp(ntg_solid_block* solid_block, ntg_cell cell_bp)
{
    assert(solid_block != NULL);
}
