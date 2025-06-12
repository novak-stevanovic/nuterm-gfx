#ifndef _NTG_COLOR_BLOCK_DEF_H_
#define _NTG_COLOR_BLOCK_DEF_H_

#include "object/ntg_color_block.h"

#ifndef __NTG_ALLOW_NTG_COLOR_BLOCK_DEF__
// #error "Only include this file when requiring the definition."
#endif // __NTG_ALLOW_NTG_COLOR_BLOCK_DEF__

#define __NTG_ALLOW_NTG_PANE_DEF__
#include "object/def/ntg_pane_def.h"
#undef __NTG_ALLOW_NTG_PANE_DEF__

struct ntg_color_block
{
    ntg_pane_t _base;

    nt_color_t __color;
};

void __ntg_color_block_init__(ntg_color_block_t* block);
void __ntg_color_block_deinit__(ntg_color_block_t* block);

#endif // _NTG_COLOR_BLOCK_DEF_H_
