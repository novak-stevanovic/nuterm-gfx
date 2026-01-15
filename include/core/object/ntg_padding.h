#ifndef _NTG_PADDING_H_
#define _NTG_PADDING_H_

#include "core/object/ntg_object.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_padding_width
{
    size_t north, east, south, west;
};

struct ntg_padding_width ntg_padding_width(
        size_t north, size_t east,
        size_t south, size_t west);

enum ntg_padding_enable_mode
{
    NTG_PADDING_ENABLE_ON_NATURAL,
    NTG_PADDING_ENABLE_ON_MIN,
    NTG_PADDING_ENABLE_ALWAYS
};

struct ntg_padding_opts
{
    ntg_padding_enable_mode mode;
    struct ntg_padding_width width;
};

struct ntg_padding_opts ntg_padding_opts_def();

struct ntg_padding
{
    ntg_object __base;

    struct ntg_padding_opts _opts;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_padding_set_opts(ntg_padding* padding, struct ntg_padding_opts opts);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_padding_init(ntg_padding* padding, ntg_object_draw_fn draw_fn);

void _ntg_padding_deinit_fn(ntg_entity* object);

struct ntg_object_measure _ntg_padding_measure_fn(
        const ntg_object* _padding,
        void* _layout_data,
        ntg_orientation orientation,
        size_t for_size,
        sarena* arena);

void _ntg_padding_constrain_fn(
        const ntg_object* _padding,
        void* _layout_data,
        ntg_orientation orientation,
        size_t size,
        ntg_object_size_map* out_sizes,
        sarena* arena);

void _ntg_padding_arrange_fn(
        const ntg_object* _padding,
        void* _layout_data,
        struct ntg_xy size,
        ntg_object_xy_map* out_positions,
        sarena* arena);

#endif // _NTG_PADDING_H_
