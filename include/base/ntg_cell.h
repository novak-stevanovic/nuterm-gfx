#ifndef _NTG_CELL_H_
#define _NTG_CELL_H_

#include <stdint.h>
#include <string.h>
#include "shared/ntg_typedef.h"
#include "shared/ntg_vecgrid.h"
#include "nt_gfx.h"
#include "shared/ntg_xy.h"

#define NTG_CELL_EMPTY ' '

/* -------------------------------------------------------------------------- */
/* CELL */
/* -------------------------------------------------------------------------- */

struct ntg_cell
{
    uint32_t codepoint;
    struct nt_gfx gfx;
};

static inline struct ntg_cell ntg_cell_default()
{
    return (struct ntg_cell) {
        .codepoint = NTG_CELL_EMPTY,
        .gfx = NT_GFX_DEFAULT
    };
}

static inline bool ntg_cell_are_equal(struct ntg_cell c1, struct ntg_cell c2)
{
    return ((c1.codepoint == c2.codepoint) && nt_gfx_are_equal(c1.gfx, c2.gfx));
}

/* -------------------------------------------------------------------------- */
/* VCELL */
/* -------------------------------------------------------------------------- */

enum ntg_vcell_type
{ 
    NTG_VCELL_FULL,
    NTG_VCELL_OVERLAY,
    NTG_VCELL_TRANSPARENT
};

struct ntg_vcell
{
    union
    {
        struct
        {
            uint32_t codepoint;
            struct nt_gfx gfx;
        } full;

        struct
        {
            uint32_t codepoint;
            nt_color fg;
            nt_style style;
        } overlay;

        struct {} transparent;
    };
    ntg_vcell_type type;
};

static inline struct ntg_vcell ntg_vcell_default()
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .full = {
            .codepoint = NTG_CELL_EMPTY,
            .gfx = NT_GFX_DEFAULT
        }
    };
}

static inline struct ntg_vcell ntg_vcell_full(uint32_t cp, struct nt_gfx gfx)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .full = {
            .codepoint = cp,
            .gfx = gfx
        }
    };
}

static inline struct ntg_vcell ntg_vcell_overlay(uint32_t cp,
        nt_color fg, nt_style style)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_OVERLAY,
        .overlay = {
            .codepoint = cp,
            .fg = fg,
            .style = style
        }
    };
}

static inline struct ntg_vcell ntg_vcell_transparent()
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_TRANSPARENT
    };
}

static inline struct ntg_vcell ntg_vcell_bg(nt_color color)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .full = {
            .codepoint = NTG_CELL_EMPTY,
            .gfx = {
                .bg = color,
                .fg = NT_COLOR_DEFAULT,
                .style = NT_STYLE_DEFAULT
            }
        }
    };
}

static inline bool ntg_vcell_are_equal(struct ntg_vcell c1, struct ntg_vcell c2)
{
    if(c1.type != c2.type) return false;

    if(c1.type == NTG_VCELL_FULL)
    {
        return ((c1.full.codepoint == c2.full.codepoint) &&
            nt_gfx_are_equal(c1.full.gfx, c2.full.gfx));
    }
    else if(c1.type == NTG_VCELL_OVERLAY)
    {
        return ((c1.overlay.codepoint == c2.overlay.codepoint) &&
            nt_color_are_equal(c1.overlay.fg, c2.overlay.fg) &&
            nt_style_are_equal(c1.overlay.style, c2.overlay.style));
    }
    else return true;
}

static inline struct ntg_cell ntg_vcell_overwrite(struct ntg_vcell overwriting, struct ntg_cell overwritten)
{
    if(overwriting.type == NTG_VCELL_FULL)
    {
        overwritten.codepoint = overwriting.full.codepoint;
        overwritten.gfx = overwriting.full.gfx;
    }
    else if(NTG_VCELL_OVERLAY)
    {
        overwritten.codepoint = overwriting.overlay.codepoint;
        overwritten.gfx.fg = overwriting.overlay.fg;
        overwritten.gfx.style = overwriting.overlay.style;
    }
    else {} // NTG_VCELL_TRANSPARENT

    return overwritten;
}

/* -------------------------------------------------------------------------- */
/* CELL VECGRID */
/* -------------------------------------------------------------------------- */

struct ntg_cell_vecgrid
{
    struct ntg_vecgrid __base; 
};

void ntg_cell_vecgrid_init(ntg_cell_vecgrid* vecgrid);
void ntg_cell_vecgrid_deinit(ntg_cell_vecgrid* vecgrid);

void ntg_cell_vecgrid_set_size(ntg_cell_vecgrid* vecgrid,
    struct ntg_xy size, struct ntg_xy size_cap);
struct ntg_xy ntg_cell_vecgrid_get_size(const ntg_cell_vecgrid* vecgrid);

static inline const struct ntg_cell*
ntg_cell_vecgrid_at(const ntg_cell_vecgrid* vecgrid, struct ntg_xy pos)
{
    if(vecgrid == NULL) return NULL;

    if(ntg_xy_is_lesser(pos, vecgrid->__base._size))
    {
        size_t idx = vecgrid->__base._size.x * pos.y + pos.x;
        return &(((const struct ntg_cell*)vecgrid->__base._data))[idx];
    }
    else
        return NULL;
}

static inline struct ntg_cell*
ntg_cell_vecgrid_at_(ntg_cell_vecgrid* vecgrid, struct ntg_xy pos)
{
    if(vecgrid == NULL) return NULL;

    if(ntg_xy_is_lesser(pos, vecgrid->__base._size))
    {
        size_t idx = vecgrid->__base._size.x * pos.y + pos.x;
        return &(((struct ntg_cell*)vecgrid->__base._data))[idx];
    }
    else
        return NULL;
}

/* -------------------------------------------------------------------------- */
/* VCELL VECGRID */
/* -------------------------------------------------------------------------- */

struct ntg_vcell_vecgrid
{
    struct ntg_vecgrid __base; 
};

void ntg_vcell_vecgrid_init(ntg_vcell_vecgrid* vecgrid);
void ntg_vcell_vecgrid_deinit(ntg_vcell_vecgrid* vecgrid);

void ntg_vcell_vecgrid_set_size(ntg_vcell_vecgrid* vecgrid,
    struct ntg_xy size, struct ntg_xy size_cap);
struct ntg_xy ntg_vcell_vecgrid_get_size(const ntg_vcell_vecgrid* vecgrid);

static inline const struct ntg_vcell*
ntg_vcell_vecgrid_at(const ntg_vcell_vecgrid* vecgrid, struct ntg_xy pos)
{
    if(vecgrid == NULL) return NULL;

    if(ntg_xy_is_lesser(pos, vecgrid->__base._size))
    {
        size_t idx = vecgrid->__base._size.x * pos.y + pos.x;
        return &(((const struct ntg_vcell*)vecgrid->__base._data))[idx];
    }
    else
        return NULL;
}

static inline struct ntg_vcell*
ntg_vcell_vecgrid_at_(ntg_vcell_vecgrid* vecgrid, struct ntg_xy pos)
{
    if(vecgrid == NULL) return NULL;

    if(ntg_xy_is_lesser(pos, vecgrid->__base._size))
    {
        size_t idx = vecgrid->__base._size.x * pos.y + pos.x;
        return &(((struct ntg_vcell*)vecgrid->__base._data))[idx];
    }
    else
        return NULL;
}

#endif // _NTG_CELL_H_
