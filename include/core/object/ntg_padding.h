#ifndef _NTG_PADDING_H_
#define _NTG_PADDING_H_

#include "core/object/ntg_object.h"

typedef struct ntg_padding ntg_padding;

struct ntg_padding_width
{
    size_t north, east, south, west;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

struct ntg_padding
{
    ntg_object __base;

    struct ntg_padding_width __width;
};

void __ntg_padding_init__(
        ntg_padding* padding,
        struct ntg_padding_width init_width,
        ntg_object_draw_fn draw_fn,
        ntg_object_deinit_fn deinit_fn,
        void* data);
void __ntg_padding_deinit__(ntg_padding* padding);

void ntg_padding_set_width(ntg_padding* padding, struct ntg_padding_width width);
struct ntg_padding_width ntg_padding_get_width(const ntg_padding* padding);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void __ntg_padding_deinit_fn(ntg_object* object);

struct ntg_object_measure __ntg_padding_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena);

void __ntg_padding_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        sarena* arena);

void __ntg_padding_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        sarena* arena);

#endif // _NTG_PADDING_H_
