#ifndef NTG_CELL_H
#define NTG_CELL_H

#include <stdint.h>
#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"
#include "thirdparty/genc.h"
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

/* ------------------------------------------------------ */
/* VCELL */
/* ------------------------------------------------------ */

enum ntg_vcell_type
{ 
    NTG_VCELL_FULL,
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
            uint8_t style;
        } overlay;

        struct 
        {
            uint8_t placeholder;
        } transparent;
    } data;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* CELL */
/* ------------------------------------------------------ */

static inline struct ntg_cell 
ntg_cell_default(void)
{
    return (struct ntg_cell) {
        .cp = ' ',
        .gfx = NT_GFX_ZERO
    };
}

static inline bool 
ntg_cell_are_eql(struct ntg_cell c1, struct ntg_cell c2)
{
    return ((c1.cp == c2.cp) && nt_gfx_are_eql(c1.gfx, c2.gfx));
}

/* ------------------------------------------------------ */
/* VCELL */
/* ------------------------------------------------------ */

static inline struct ntg_vcell
ntg_vcell_new(enum ntg_vcell_type type, struct nt_gfx gfx, uint32_t cp)
{
    struct ntg_vcell rval = {0};
    rval.type = type;
    if(type == NTG_VCELL_FULL)
    {
        rval.data.full.gfx = gfx;
        rval.data.full.cp = cp;
    }
    else if(type == NTG_VCELL_OVERLAY)
    {
        rval.data.overlay.fg = gfx.fg;
        rval.data.overlay.style = gfx.style;
        rval.data.overlay.cp = cp;
    }

    return rval;
}

static inline struct ntg_vcell 
ntg_vcell_new_default(void)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .data.full = { .cp = ' ', .gfx = NT_GFX_ZERO }
    };
}

static inline struct ntg_vcell 
ntg_vcell_new_full(uint32_t cp, struct nt_gfx gfx)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .data.full = { .cp = cp, .gfx = gfx }
    };
}

static inline struct ntg_vcell 
ntg_vcell_new_overlay(uint32_t cp, nt_color fg, uint8_t style)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_OVERLAY,
        .data.overlay = { .cp = cp, .fg = fg, .style = style }
    };
}

static inline struct ntg_vcell 
ntg_vcell_new_transparent(void)
{
    return (struct ntg_vcell) { .type = NTG_VCELL_TRANSPARENT };
}

static inline struct ntg_vcell 
ntg_vcell_new_full_bg(nt_color color)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .data.full = {
            .cp = ' ',
            .gfx = {
                .bg = color,
                .fg = {0},
                .style = 0
            }
        }
    };
}

static inline bool
ntg_vcell_are_eql(struct ntg_vcell c1, struct ntg_vcell c2)
{
    if(c1.type != c2.type) return false;

    if(c1.type == NTG_VCELL_FULL)
    {
        return ((c1.data.full.cp == c2.data.full.cp) &&
            nt_gfx_are_eql(c1.data.full.gfx, c2.data.full.gfx));
    }
    else if(c1.type == NTG_VCELL_OVERLAY)
    {
        return ((c1.data.overlay.cp == c2.data.overlay.cp) &&
            nt_color_are_eql(c1.data.overlay.fg, c2.data.overlay.fg) &&
            (c1.data.overlay.style == c2.data.overlay.style));
    }
    else return true;
}


static inline struct ntg_cell
ntg_vcell_overwrite(struct ntg_vcell overwriting, struct ntg_cell overwritten)
{
    if(overwriting.type == NTG_VCELL_FULL)
    {
        overwritten.cp = overwriting.data.full.cp;
        overwritten.gfx = overwriting.data.full.gfx;
    }
    else if(overwriting.type == NTG_VCELL_OVERLAY)
    {
        overwritten.cp = overwriting.data.overlay.cp;
        overwritten.gfx.fg = overwriting.data.overlay.fg;
        overwritten.gfx.style = overwriting.data.overlay.style;
    }
    else {} 

    return overwritten;
}

#endif // NTG_CELL_H
