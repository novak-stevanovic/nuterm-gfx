#ifndef _NTG_BORDER_H_
#define _NTG_BORDER_H_

#include "core/object/ntg_padding.h"

typedef struct ntg_def_border ntg_def_border;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

struct ntg_def_border_opts
{
    ntg_cell top_left, top, top_right,
             right, bottom_right, bottom,
             bottom_left, left, padding;
};

struct ntg_def_border
{
    ntg_padding __base;

    struct ntg_def_border_opts __opts;
};

void _ntg_def_border_init_(
        ntg_def_border* def_border,
        struct ntg_def_border_opts opts,
        struct ntg_padding_width init_width);
void _ntg_def_border_deinit_(ntg_def_border* def_border);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_border_deinit_fn(ntg_object* object);

void _ntg_def_border_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena);

#endif // _NTG_BORDER_H_
