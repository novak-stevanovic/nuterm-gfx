#ifndef _NTG_COLOR_BLOCK_H_
#define _NTG_COLOR_BLOCK_H_

#include "object/ntg_object.h"

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

struct ntg_measure_result _ntg_color_block_measure_fn(const ntg_object* _block,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context);

void _ntg_color_block_arrange_drawing_fn(const ntg_object* _block,
        struct ntg_xy size, ntg_object_drawing* out_drawing);

#endif // _NTG_COLOR_BLOCK_H_
