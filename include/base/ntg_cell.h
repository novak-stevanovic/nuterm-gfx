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
    if(ntg_cell_cp_is_ws(cp1) && ntg_cell_cp_is_ws(cp2))
    {
        return nt_color_are_eql(c1.gfx.bg, c2.gfx.bg);
    }
    else if(!ntg_cell_cp_is_ws(cp1) && !ntg_cell_cp_is_ws(cp2))
    {
        return ((cp1 == cp2) && nt_gfx_are_eql(c1.gfx, c2.gfx));
    }
    else return false;
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
ntg_vcell_new_full_bg(nt_color bg)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .data.full = {
            .cp = 0,
            .gfx = nt_gfx_new(NT_COLOR_ZERO, bg, 0)
        }
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

    if(c1.type == NTG_VCELL_FULL)
    {
        struct ntg_cell cell1 = ntg_cell_new(c1.data.full.cp, c1.data.full.gfx);
        struct ntg_cell cell2 = ntg_cell_new(c2.data.full.cp, c2.data.full.gfx);
        return ntg_cell_are_eql_render(cell1, cell2);
    }
    else if(c1.type == NTG_VCELL_OVERLAY)
    {
        struct nt_gfx gfx1 = nt_gfx_new(
                c1.data.overlay.fg,
                NT_COLOR_ZERO,
                c1.data.overlay.style);
        struct nt_gfx gfx2 = nt_gfx_new(
                c2.data.overlay.fg,
                NT_COLOR_ZERO,
                c2.data.overlay.style);
        struct ntg_cell cell1 = ntg_cell_new(c1.data.overlay.cp, gfx1);
        struct ntg_cell cell2 = ntg_cell_new(c2.data.overlay.cp, gfx2);
        return ntg_cell_are_eql_render(cell1, cell2);
    }
    else return true;
}

static inline struct ntg_cell
ntg_vcell_overwrite(struct ntg_vcell overwriting, struct ntg_cell overwritten)
{
    if(overwriting.type == NTG_VCELL_FULL)
    {
        overwritten.cp = ntg_cell_cp_normalize(overwriting.data.full.cp);
        overwritten.gfx = overwriting.data.full.gfx;
    }
    else if(overwriting.type == NTG_VCELL_OVERLAY)
    {
        overwritten.cp = ntg_cell_cp_normalize(overwriting.data.overlay.cp);
        overwritten.gfx.fg = overwriting.data.overlay.fg;
        overwritten.gfx.style = overwriting.data.overlay.style;
    }
    else {} 

    return overwritten;
}

#endif // NTG_CELL_H
