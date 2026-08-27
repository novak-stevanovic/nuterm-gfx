#ifndef NTG_BORDER_9X_H
#define NTG_BORDER_9X_H

#include "shared/ntg_shared.h"
#include "core/object/border/ntg_border_style.h"
#include "nt_gfx.h"
#include "base/ntg_cell.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_border_9x_sym
{
    uint32_t top_left, top, top_right, right,
             bottom_right, bottom, bottom_left,
             left, padding;
};

struct ntg_border_9x
{
    ntg_border_style _base;

    struct
    {
        bool overlay;
        union
        {
            struct
            {
                struct nt_gfx gfx;
            } full;

            struct
            {
                nt_color fg;
                nt_style style;
            } overlay;

        } data;

        struct ntg_border_9x_sym symbols;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

NTG_API int
ntg_border_9x_init_monochrome(ntg_border_9x* bs, nt_color color);

/* ------------------------------------------------------ */
/* BASIC */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_basic(ntg_border_9x* bs, uint32_t cp, struct nt_gfx gfx);

NTG_API int
ntg_border_9x_init_basic_overlay(
        ntg_border_9x* bs,
        uint32_t cp,
        nt_color fg,
        nt_style style);

/* ------------------------------------------------------ */
/* BASIC EDGE */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_basic_edge(ntg_border_9x* bs, struct nt_gfx gfx, uint32_t cp);

NTG_API int
ntg_border_9x_init_basic_edge_overlay(
        ntg_border_9x* bs,
        nt_color fg,
        nt_style style,
        uint32_t cp);

/* ------------------------------------------------------ */
/* SINGLE */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_single(ntg_border_9x* bs, struct nt_gfx gfx);

NTG_API int
ntg_border_9x_init_single_overlay(ntg_border_9x* bs, nt_color fg, nt_style style);

/* ------------------------------------------------------ */
/* DOUBLE */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_double(ntg_border_9x* bs, struct nt_gfx gfx);

NTG_API int
ntg_border_9x_init_double_overlay(ntg_border_9x* bs, nt_color fg, nt_style style);

/* ------------------------------------------------------ */
/* ROUNDED */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_rounded(ntg_border_9x* bs, struct nt_gfx gfx);

NTG_API int
ntg_border_9x_init_rounded_overlay(ntg_border_9x* bs, nt_color fg, nt_style style);

/* ------------------------------------------------------ */
/* HEAVY */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_heavy(ntg_border_9x* bs, struct nt_gfx gfx);

NTG_API int
ntg_border_9x_init_heavy_overlay(ntg_border_9x* bs, nt_color fg, nt_style style);

/* ------------------------------------------------------ */
/* DASHED */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_dashed(ntg_border_9x* bs, struct nt_gfx gfx);

NTG_API int
ntg_border_9x_init_dashed_overlay(ntg_border_9x* bs, nt_color fg, nt_style style);

/* ------------------------------------------------------ */
/* ASCII */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_ascii(ntg_border_9x* bs, struct nt_gfx gfx);

NTG_API int
ntg_border_9x_init_ascii_overlay(ntg_border_9x* bs, nt_color fg, nt_style style);

/* ------------------------------------------------------ */
/* TRANSPARENT */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_transparent(ntg_border_9x* bs);

/* ------------------------------------------------------ */
/* CUSTOM */
/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_init_custom(
        ntg_border_9x* bs,
        struct nt_gfx gfx,
        const struct ntg_border_9x_sym* symbols);

NTG_API int
ntg_border_9x_init_custom_overlay(
        ntg_border_9x* bs,
        nt_color fg,
        nt_style style,
        const struct ntg_border_9x_sym* symbols);

/* ------------------------------------------------------ */

NTG_API int
ntg_border_9x_deinit(ntg_border_9x* bs);

#endif // NTG_BORDER_9X_H
