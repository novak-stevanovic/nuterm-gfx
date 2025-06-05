#ifndef _NTG_COLOR_BLOCK_H_
#define _NTG_COLOR_BLOCK_H_

#include "nt_gfx.h"
#include "object/ntg_object_fwd.h"

ntg_color_block_t* ntg_color_block_new(nt_color_t color);
void ntg_color_block_destroy(ntg_color_block_t* block);

void ntg_color_block_set_color(ntg_color_block_t* block, nt_color_t color);
nt_color_t ntg_color_block_get_color(ntg_color_block_t* block);

#endif // _NTG_COLOR_BLOCK_H_
