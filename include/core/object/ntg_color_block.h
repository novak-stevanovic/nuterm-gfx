#ifndef _NTG_COLOR_BLOCK_H_
#define _NTG_COLOR_BLOCK_H_

#include "core/object/ntg_object.h"

#define NTG_CBLOCK(color_block_ptr) ((ntg_color_block*)(color_block_ptr))

typedef struct ntg_color_block
{
    ntg_object __base;
    nt_color __color;
} ntg_color_block;

void __ntg_color_block_init__(
        ntg_color_block* color_block,
        nt_color color,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn);
void __ntg_color_block_deinit__(ntg_color_block* color_block);

nt_color ntg_color_block_get_color(ntg_color_block* color_block);
void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color);

struct ntg_measure_output __ntg_color_block_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_context* context);

void __ntg_color_block_draw_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        ntg_drawing* out_drawing);

#endif // _NTG_COLOR_BLOCK_H_
