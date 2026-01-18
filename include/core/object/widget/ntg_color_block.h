#ifndef NTG_COLOR_BLOCK_H
#define NTG_COLOR_BLOCK_H

#include "core/object/widget/ntg_widget.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_color_block
{
    ntg_widget __base;
    nt_color __color;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_color_block* ntg_color_block_new(ntg_entity_system* system);
void ntg_color_block_init(ntg_color_block* color_block);

void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_color_block_deinit_fn(ntg_entity* entity);

struct ntg_object_measure _ntg_color_block_measure_fn(
        const ntg_widget* _block,
        void* _layout_data,
        ntg_orient orient,
        bool constrained,
        sarena* arena);

void _ntg_color_block_draw_fn(
        const ntg_widget* _block,
        void* _layout_data,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena);

#endif // NTG_COLOR_BLOCK_H
