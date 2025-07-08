#ifndef _NTG_COLOR_BLOCK_H_
#define _NTG_COLOR_BLOCK_H_

#include "nt_gfx.h"
#include "object/ntg_solid_block.h"

#define NTG_COLOR_BLOCK(cb_ptr) ((ntg_color_block*)(cb_ptr))

typedef struct ntg_color_block
{
    ntg_solid_block __base;

    nt_color _color;
} ntg_color_block;

void __ntg_color_block_init__(ntg_color_block* block, nt_color color);
void __ntg_color_block_deinit__(ntg_color_block* block);

ntg_color_block* ntg_color_block_new(nt_color color);
void ntg_color_block_destroy(ntg_color_block* block);

void ntg_color_block_set_color(ntg_color_block* block, nt_color color);

#endif // _NTG_COLOR_BLOCK_H_
