#ifndef NTG_BORDER_STYLE_H
#define NTG_BORDER_STYLE_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"
#include "nt_gfx.h"
#include "base/ntg_cell.h"

struct ntg_border_style
{
    void* data;
    void  (*draw_fn)(void* data,
            struct ntg_xy size,
            struct ntg_insets border_size,
            ntg_object_tmp_drawing* out_drawing);
    void  (*free_fn)(void* data);
};

/* -------------------------------------------------------------------------- */
/* DEFAULT STYLE FACTORY */
/* -------------------------------------------------------------------------- */

// Data for default border type
struct ntg_border_def_style_dt
{
    struct ntg_vcell top_left, top,
    top_right, right, bottom_right,
    bottom, bottom_left, left, padding;
};

struct ntg_border_style
ntg_border_def_style_new(struct ntg_border_def_style_dt data);

struct ntg_border_def_style_dt
ntg_border_def_style_monochrome(struct nt_color color);

struct ntg_border_def_style_dt
ntg_border_def_style_uniform(struct nt_gfx gfx, uint32_t codepoint);

struct ntg_border_def_style_dt
ntg_border_def_style_uniform_edge(struct nt_gfx gfx, uint32_t codepoint);

struct ntg_border_def_style_dt
ntg_border_def_style_single(struct nt_gfx gfx);

struct ntg_border_def_style_dt
ntg_border_def_style_double(struct nt_gfx gfx);

struct ntg_border_def_style_dt
ntg_border_def_style_rounded(struct nt_gfx gfx);

struct ntg_border_def_style_dt
ntg_border_def_style_heavy(struct nt_gfx gfx);

struct ntg_border_def_style_dt
ntg_border_def_style_dashed(struct nt_gfx gfx);

struct ntg_border_def_style_dt
ntg_border_def_style_ascii(struct nt_gfx gfx);

struct ntg_border_def_style_dt
ntg_border_def_style_transparent(struct nt_gfx gfx);

#endif // NTG_BORDER_STYLE_H
