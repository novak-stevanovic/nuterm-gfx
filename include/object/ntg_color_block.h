#ifndef _NTG_COLOR_BLOCK_H_
#define _NTG_COLOR_BLOCK_H_

#include "nt_gfx.h"

typedef struct ntg_color_block ntg_color_block;

ntg_color_block* ntg_color_block_new(nt_color color);
void ntg_color_block_destroy(ntg_color_block* block);

void ntg_color_block_set_color(ntg_color_block* block, nt_color color);
nt_color ntg_color_block_get_color(ntg_color_block* block);

#endif // _NTG_COLOR_BLOCK_H_
