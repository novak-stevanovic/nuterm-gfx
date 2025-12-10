#ifndef _NTG_PADDING_H_
#define _NTG_PADDING_H_

#include "core/object/ntg_object.h"

typedef struct ntg_padding ntg_padding;

struct ntg_padding_width
{
    size_t north, east, south, west;
};

struct ntg_padding
{
    ntg_object __base;

    struct ntg_padding_width __width;
};

void __ntg_padding_init__(
        ntg_padding* padding,
        ntg_draw_fn draw_fn,
        struct ntg_padding_width init_width,
        void* data);
void __ntg_padding_deinit__(ntg_padding* padding);

void ntg_padding_set_width(ntg_padding* padding, struct ntg_padding_width width);
struct ntg_padding_width ntg_padding_get_width(const ntg_padding* padding);

struct ntg_measure_out __ntg_padding_measure_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_ctx* ctx,
        sarena* arena);

void __ntg_padding_constrain_fn(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        const ntg_constrain_ctx* constrain_ctx,
        const ntg_measure_ctx* measure_ctx,
        ntg_constrain_out* out,
        sarena* arena);

void __ntg_padding_arrange_fn(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_ctx* ctx,
        ntg_arrange_out* out,
        sarena* arena);

#endif // _NTG_PADDING_H_
