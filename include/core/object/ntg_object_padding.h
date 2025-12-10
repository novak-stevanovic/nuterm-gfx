#ifndef _NTG_OBJECT_PADDING_H_
#define _NTG_OBJECT_PADDING_H_

#include "core/object/ntg_object.h"

typedef struct ntg_object_padding ntg_object_padding;

struct ntg_object_padding
{
    ntg_object __base;

    size_t __north_size, __east_size,
           __south_size, __west_size;
};

void __ntg_object_padding_init__(ntg_object_padding* padding, ntg_draw_fn draw_fn, void* data);
void __ntg_object_padding_deinit__(ntg_object_padding* padding);

struct ntg_measure_out __ntg_object_padding_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_ctx* ctx,
        sarena* arena);

void __ntg_object_padding_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        const ntg_constrain_ctx* constrain_ctx,
        const ntg_measure_ctx* measure_ctx,
        ntg_constrain_out* out,
        sarena* arena);

void __ntg_object_padding_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_ctx* ctx,
        ntg_arrange_out* out,
        sarena* arena);

// void __ntg_object_padding_draw_fn(
//         const ntg_drawable* drawable,
//         struct ntg_xy size,
//         ntg_drawing* out_drawing,
//         sarena* arena);

#endif // _NTG_OBJECT_PADDING_H_
