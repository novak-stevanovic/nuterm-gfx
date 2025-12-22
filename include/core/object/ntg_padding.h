#ifndef _NTG_PADDING_H_
#define _NTG_PADDING_H_

#include "core/object/ntg_object.h"

typedef struct ntg_padding ntg_padding;

struct ntg_padding_width
{
    size_t north, east, south, west;
};

struct ntg_padding_width ntg_padding_width(size_t north,
        size_t east, size_t south, size_t west);

typedef enum ntg_padding_type
{
    NTG_PADDING_PADDING,
    NTG_PADDING_BORDER
} ntg_padding_type;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

struct ntg_padding
{
    ntg_object __base;

    struct ntg_padding_width __width;
};

void _ntg_padding_init_(
        ntg_padding* padding,
        ntg_padding_type padding_type,
        struct ntg_padding_width init_width,
        ntg_object_draw_fn draw_fn,
        ntg_object_deinit_fn deinit_fn,
        ntg_object_container* container);

void ntg_padding_set_width(ntg_padding* padding, struct ntg_padding_width width);
struct ntg_padding_width ntg_padding_get_width(const ntg_padding* padding);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

struct ntg_padding_ldata
{
    struct ntg_padding_width final_width;
    void* data;
};

void _ntg_padding_deinit_fn(ntg_object* object);

void* _ntg_padding_layout_init_fn(const ntg_object* object);
void _ntg_padding_layout_deinit_fn(const ntg_object* object, void* layout_data);

struct ntg_object_measure _ntg_padding_measure_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        void* _layout_data,
        sarena* arena);

void _ntg_padding_constrain_fn(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        void* _layout_data,
        sarena* arena);

void _ntg_padding_arrange_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        void* _layout_data,
        sarena* arena);

#endif // _NTG_PADDING_H_
