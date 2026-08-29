#ifndef NTG_CELL_H
#define NTG_CELL_H

#include <stdint.h>
#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"
#include "thirdparty/genc.h"
#include "thirdparty/uconv.h"
#include "nt_gfx.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* CELL */
/* ------------------------------------------------------ */

struct ntg_cell
{
    uint32_t cp;
    struct nt_gfx gfx;
};

static const struct ntg_cell NTG_CELL_ZERO = {0};

/* ------------------------------------------------------ */
/* VCELL */
/* ------------------------------------------------------ */

enum ntg_vcell_type
{ 
    NTG_VCELL_FULL = 0,
    NTG_VCELL_OVERLAY,
    NTG_VCELL_FULL_BG,
    NTG_VCELL_OVERLAY_BG,
    NTG_VCELL_TRANSPARENT
};

struct ntg_vcell
{
    enum ntg_vcell_type type;
    union
    {
        struct
        {
            uint32_t cp;
            struct nt_gfx gfx;
        } full;
        struct
        {
            uint32_t cp;
            nt_color fg;
            nt_style style;
        } overlay;

        struct
        {
            nt_color fg, bg; /* Both needed in case of NT_STYLE_REVERSE */
            bool reverse;
        } full_bg;

        struct
        {
            nt_color fg; /* Needed in case of NT_STYLE_REVERSE */
            bool reverse;
        } overlay_bg;

        struct
        {
            uint8_t _placeholder;
        } transparent;
    } data;
};

static const struct ntg_vcell NTG_VCELL_ZERO = {0};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* Control/whitespace and invalid codepoints normalize to 0 */
static inline bool
ntg_cell_cp_is_ws(uint32_t cp)
{
    return ((cp <= 32) || (!uc_utf32_is_in_range(cp, 0)));
}

static inline uint32_t
ntg_cell_cp_normalize(uint32_t cp)
{
    return (!ntg_cell_cp_is_ws(cp) ? cp : 0);
}

/* ------------------------------------------------------ */
/* CELL */
/* ------------------------------------------------------ */

static inline struct ntg_cell
ntg_cell_new(uint32_t cp, struct nt_gfx gfx)
{
    return (struct ntg_cell) {
        .cp = ntg_cell_cp_normalize(cp),
        .gfx = gfx
    };
}

static inline bool 
ntg_cell_are_eql_render(struct ntg_cell c1, struct ntg_cell c2)
{
    uint32_t cp1 = ntg_cell_cp_normalize(c1.cp);
    uint32_t cp2 = ntg_cell_cp_normalize(c2.cp);

    return (cp1 == cp2) && nt_gfx_are_eql(c1.gfx, c2.gfx);
}

/* ------------------------------------------------------ */
/* VCELL */
/* ------------------------------------------------------ */

static inline struct ntg_vcell
ntg_vcell_new_full(uint32_t cp, struct nt_gfx gfx)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .data.full = { .cp = ntg_cell_cp_normalize(cp), .gfx = gfx }
    };
}

static inline struct ntg_vcell
ntg_vcell_new_overlay(uint32_t cp, nt_color fg, nt_style style)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_OVERLAY,
        .data.overlay = {
            .cp = ntg_cell_cp_normalize(cp),
            .fg = fg,
            .style = style
        }
    };
}

static inline struct ntg_vcell
ntg_vcell_new_full_bg(nt_color fg, nt_color bg, bool reverse)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL_BG,
        .data.full_bg = {
            .fg = fg,
            .bg = bg,
            .reverse = reverse
        }
    };
}

static inline struct ntg_vcell
ntg_vcell_new_overlay_bg(nt_color fg, bool reverse)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_OVERLAY_BG,
        .data.overlay_bg = {
            .fg = fg,
            .reverse = reverse
        }
    };
}

static inline struct ntg_vcell 
ntg_vcell_new_transparent(void)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_TRANSPARENT,
        .data.transparent = {0}
    };
}

static inline bool
ntg_vcell_are_eql(struct ntg_vcell c1, struct ntg_vcell c2)
{
    if(c1.type != c2.type) return false;

    uint32_t cp1, cp2;
    struct nt_gfx gfx1, gfx2;
    nt_color fg1, fg2, bg1, bg2;
    nt_style s1, s2;
    bool r1, r2;
    switch(c1.type)
    {
        case NTG_VCELL_FULL:
            cp1 = ntg_cell_cp_normalize(c1.data.full.cp);
            cp2 = ntg_cell_cp_normalize(c2.data.full.cp);
            gfx1 = c1.data.full.gfx;
            gfx2 = c2.data.full.gfx;

            return ((cp1 == cp2) && nt_gfx_are_eql(gfx1, gfx2));

        case NTG_VCELL_OVERLAY:
            cp1 = ntg_cell_cp_normalize(c1.data.overlay.cp);
            cp2 = ntg_cell_cp_normalize(c2.data.overlay.cp);
            fg1 = c1.data.overlay.fg;
            fg2 = c2.data.overlay.fg;
            s1 = c1.data.overlay.style;
            s2 = c2.data.overlay.style;

            return ((cp1 == cp2) && nt_color_are_eql(fg1, fg2) && (s1 == s2));

        case NTG_VCELL_FULL_BG:
            fg1 = c1.data.full_bg.fg;
            fg2 = c2.data.full_bg.fg;
            bg1 = c1.data.full_bg.bg;
            bg2 = c2.data.full_bg.bg;
            r1 = c1.data.full_bg.reverse;
            r2 = c2.data.full_bg.reverse;

            return (nt_color_are_eql(fg1, fg2) && nt_color_are_eql(bg1, bg2) && (r1 == r2));

        case NTG_VCELL_OVERLAY_BG:
            fg1 = c1.data.overlay_bg.fg;
            fg2 = c2.data.overlay_bg.fg;
            r1 = c1.data.overlay_bg.reverse;
            r2 = c2.data.overlay_bg.reverse;

            return (nt_color_are_eql(fg1, fg2) && (r1 == r2));

        case NTG_VCELL_TRANSPARENT:

            return true;

        default:
            return false;
    }
}

static inline struct ntg_cell
ntg_vcell_overwrite(struct ntg_vcell over, struct ntg_cell base)
{
    bool reverse;
    nt_style style;
    switch(over.type)
    {
        case NTG_VCELL_FULL:
            base.cp = ntg_cell_cp_normalize(over.data.full.cp);
            base.gfx = over.data.full.gfx;
            break;
        case NTG_VCELL_OVERLAY:
            base.cp = ntg_cell_cp_normalize(over.data.overlay.cp);
            base.gfx.fg = over.data.overlay.fg;
            base.gfx.style = over.data.overlay.style;
            break;
        case NTG_VCELL_FULL_BG:
            base.cp = 0;
            reverse = over.data.full_bg.reverse;
            style = (reverse ? NT_STYLE_REVERSE : 0);
            base.gfx = nt_gfx_new(over.data.full_bg.fg, over.data.full_bg.bg, style);
            break;
        case NTG_VCELL_OVERLAY_BG:
            base.cp = 0;
            reverse = over.data.overlay_bg.reverse;
            style = (reverse ? NT_STYLE_REVERSE : 0);
            base.gfx = nt_gfx_new(over.data.overlay_bg.fg, base.gfx.bg, style);
            break;
        case NTG_VCELL_TRANSPARENT:
            break;
    }

    return base;
}

#endif // NTG_CELL_H
