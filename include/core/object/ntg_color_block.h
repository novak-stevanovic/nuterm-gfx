#ifndef _NTG_COLOR_BLOCK_H_
#define _NTG_COLOR_BLOCK_H_

#include "core/object/ntg_object.h"

typedef struct ntg_color_block ntg_color_block;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

void __ntg_color_block_init__(
        ntg_color_block* color_block,
        nt_color color,
        ntg_object_process_key_fn process_key_fn,
        ntg_object_focus_fn on_focus_fn,
        ntg_object_unfocus_fn on_unfocus_fn,
        void* data);
void __ntg_color_block_deinit__(ntg_color_block* color_block);

nt_color ntg_color_block_get_color(const ntg_color_block* color_block);
void ntg_color_block_set_color(ntg_color_block* color_block, nt_color color);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_color_block
{
    ntg_object __base;
    nt_color __color;
};

void __ntg_color_block_deinit_fn(ntg_object* object);

struct ntg_object_measure __ntg_color_block_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena);

void __ntg_color_block_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena);

#endif // _NTG_COLOR_BLOCK_H_
