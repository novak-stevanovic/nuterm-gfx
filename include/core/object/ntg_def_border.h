#ifndef _NTG_DEF_BORDER_H_
#define _NTG_DEF_BORDER_H_

#include "core/object/ntg_padding.h"

typedef struct ntg_def_border ntg_def_border;

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

struct ntg_def_border_style
{
    ntg_cell top_left, top, top_right,
             right, bottom_right, bottom,
             bottom_left, left, padding;
};

struct ntg_def_border
{
    ntg_padding __base;

    struct ntg_def_border_style __style;
};

void _ntg_def_border_init_(
        ntg_def_border* def_border,
        struct ntg_def_border_style style,
        struct ntg_padding_width init_width,
        ntg_entity_group* group,
        ntg_entity_system* system);

/* ------------------------------------------------------ */
/* PRESETS */
/* ------------------------------------------------------ */

void _ntg_def_border_init_monochrome_(
        ntg_def_border* def_border,
        nt_color color,
        struct ntg_padding_width init_width,
        ntg_entity_group* group,
        ntg_entity_system* system);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_def_border_deinit_fn(ntg_entity* entity);

void _ntg_def_border_draw_fn(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        void* _layout_data,
        sarena* arena);

#endif // _NTG_DEF_BORDER_H_
