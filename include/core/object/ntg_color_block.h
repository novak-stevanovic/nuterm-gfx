#ifndef _NTG_COLOR_BLOCK_H_
#define _NTG_COLOR_BLOCK_H_

#include "core/object/ntg_object.h"

#define NTG_CBLOCK(color_block_ptr) ((ntg_color_block*)(color_block_ptr))

typedef struct ntg_color_block
{
    ntg_object __base;
    nt_color __color;
} ntg_color_block;

void __ntg_color_block_init__(ntg_color_block* color_block, nt_color color);
void __ntg_color_block_deinit__(ntg_color_block* color_block);

ntg_color_block* ntg_color_block_new(nt_color color);
void ntg_color_block_destroy(ntg_color_block* color_block);

nt_color ntg_color_block_get_color(ntg_color_block* color_block);
void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color);

#endif // _NTG_COLOR_BLOCK_H_
