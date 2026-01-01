#ifndef _NTG_COLOR_BLOCK_H_
#define _NTG_COLOR_BLOCK_H_

#include "core/object/ntg_object.h"

typedef struct ntg_color_block ntg_color_block;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_color_block
{
    ntg_object __base;
    nt_color __color;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_color_block* ntg_color_block_new(ntg_entity_system* system);
void _ntg_color_block_init_(ntg_color_block* color_block);

void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_color_block_deinit_fn(ntg_entity* entity);

struct ntg_object_measure _ntg_color_block_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* layout_data,
        sarena* arena);

void _ntg_color_block_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* layout_data,
        sarena* arena);

#endif // _NTG_COLOR_BLOCK_H_
