#ifndef _NTG_SOLID_BLOCK_H_
#define _NTG_SOLID_BLOCK_H_

#include "base/ntg_cell.h"
#include "object/ntg_pane.h"

#define NTG_SOLID_BLOCK(sb_ptr) ((ntg_solid_block*)(sb_ptr))

typedef struct ntg_solid_block
{
    ntg_pane __base;

    ntg_cell _cell_bp;
} ntg_solid_block;

void __ntg_solid_block_init__(ntg_solid_block* solid_block, ntg_cell cell_bp);
void __ntg_solid_block_deinit__(ntg_solid_block* solid_block);

ntg_solid_block* ntg_solid_block_new(ntg_cell cell_bp);
void ntg_solid_block_destroy(ntg_solid_block* solid_block);

void ntg_solid_block_set_cell_bp(ntg_solid_block* solid_block, ntg_cell cell_bp);

#endif // _NTG_SOLID_BLOCK_H_
