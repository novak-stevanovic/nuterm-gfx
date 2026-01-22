#ifndef NTG_DEF_BORDER_H
#define NTG_DEF_BORDER_H

#include "core/object/decorator/ntg_decorator.h"
#include "nt_gfx.h"

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
    ntg_decorator __base;

    struct ntg_def_border_style _style;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_def_border* ntg_def_border_new(ntg_entity_system* system);
void ntg_def_border_init(ntg_def_border* def_border);

void ntg_def_border_set_style(ntg_def_border* border,
                              struct ntg_def_border_style style);

/* ------------------------------------------------------ */
/* PRESETS */
/* ------------------------------------------------------ */

struct ntg_def_border_style 
ntg_def_border_style_def();

struct ntg_def_border_style 
ntg_def_border_style_monochrome(nt_color color);

struct ntg_def_border_style
ntg_def_border_style_uniform(struct nt_gfx gfx, uint32_t codepoint);

struct ntg_def_border_style
ntg_def_border_style_uniform_edge(struct nt_gfx gfx, uint32_t codepoint);

struct ntg_def_border_style
ntg_def_border_style_single(struct nt_gfx gfx);

struct ntg_def_border_style
ntg_def_border_style_double(struct nt_gfx gfx);

struct ntg_def_border_style
ntg_def_border_style_rounded(struct nt_gfx gfx);

struct ntg_def_border_style
ntg_def_border_style_heavy(struct nt_gfx gfx);

struct ntg_def_border_style
ntg_def_border_style_dashed(struct nt_gfx gfx);

struct ntg_def_border_style
ntg_def_border_style_ascii(struct nt_gfx gfx);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void ntg_def_border_deinit(ntg_def_border* border);

void _ntg_def_border_draw_fn(
        const ntg_object* _border,
        void* _layout_data,
        ntg_tmp_object_drawing* out_drawing,
        sarena* arena);

#endif // NTG_DEF_BORDER_H
