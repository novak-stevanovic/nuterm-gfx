#ifndef _NTG_DEF_BORDER_H_
#define _NTG_DEF_BORDER_H_

#include "core/object/ntg_padding.h"

typedef struct ntg_def_border ntg_def_border;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_def_border_style
{
    struct ntg_vcell top_left, top, top_right,
             right, bottom_right, bottom,
             bottom_left, left, padding;
};

struct ntg_def_border
{
    ntg_padding __base;

    struct ntg_def_border_style __style;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_border* ntg_def_border_new(ntg_entity_system* system);
void _ntg_def_border_init_(ntg_def_border* def_border);

struct ntg_def_border_style ntg_def_border_get_style(const ntg_def_border* border);
void ntg_def_border_set_style(ntg_def_border* border, struct ntg_def_border_style style);

/* ------------------------------------------------------ */
/* PRESETS */
/* ------------------------------------------------------ */

struct ntg_def_border_style ntg_def_border_style_def();
struct ntg_def_border_style ntg_def_border_style_monochrome(nt_color color);

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
