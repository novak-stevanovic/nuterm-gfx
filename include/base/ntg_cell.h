#ifndef NTG_CELL_H
#define NTG_CELL_H

#include <stdint.h>
#include "shared/ntg_shared.h"
#include "base/ntg_vecgrid.h"
#include "nt_gfx.h"

/* ========================================================================== */
/* PUBLIC - TYPES AND FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* CELL */
/* ------------------------------------------------------ */

struct ntg_cell
{
    uint32_t cp;
    struct nt_gfx gfx;
};

/* Creates the default rendered cell: a space character with default graphics.
 *
 * RETURN VALUE:
 * The default cell value. */
static inline struct ntg_cell 
ntg_cell_default()
{
    return (struct ntg_cell) {
        .cp = ' ',
        .gfx = NT_GFX_DEFAULT
    };
}

/* Compares two rendered cells, including their code points and graphics. */
static inline bool 
ntg_cell_are_equal(struct ntg_cell c1, struct ntg_cell c2)
{
    return ((c1.cp == c2.cp) && nt_gfx_are_equal(c1.gfx, c2.gfx));
}

/* ------------------------------------------------------ */
/* CELL VECGRID */
/* ------------------------------------------------------ */

struct ntg_cell_vecgrid
{
    struct ntg_vecgrid __base; 
};

/* Initializes an empty rendered-cell grid. No cell storage is allocated until
 * the grid is resized. */
NTG_API void
ntg_cell_vecgrid_init(ntg_cell_vecgrid* vecgrid);
/* Releases storage owned by a rendered-cell grid and resets its base grid.
 * Passing `NULL` has no effect. */
NTG_API void
ntg_cell_vecgrid_deinit(ntg_cell_vecgrid* vecgrid);

/* Resizes a rendered-cell grid, preserves overlapping cells, and fills newly
 * added cells with the default cell. `size_cap` caps only extra reserved
 * capacity, not the requested dimensions.
 *
 * ERROR CODES:
 * - `NTG_ERR_ALLOC_FAIL`: `vecgrid` is `NULL` or storage allocation fails.
 * - `NTG_ERR_OUT_OF_BOUNDS`: a dimension product or byte allocation size would
 *   overflow `size_t`. */
NTG_API void
ntg_cell_vecgrid_set_size(
        ntg_cell_vecgrid* vecgrid,
        struct ntg_xy size,
        struct ntg_xy size_cap,
        int* out_status);
/* Gets the current dimensions of a rendered-cell grid.
 *
 * RETURN VALUE:
 * The grid size, or `(0, 0)` when `vecgrid` is `NULL`. */
NTG_API struct ntg_xy
ntg_cell_vecgrid_get_size(const ntg_cell_vecgrid* vecgrid);

/* Reads a rendered cell at `pos` without modifying the grid.
 *
 * RETURN VALUE:
 * The stored cell, or the default cell when the grid is `NULL` or `pos` is
 * outside it. */
static inline struct ntg_cell
ntg_cell_vecgrid_get(const ntg_cell_vecgrid* vecgrid, struct ntg_xy pos)
{
    if(!vecgrid)
        return ntg_cell_default();

    if(ntg_xy_is_lesser(pos, vecgrid->__base._size))
    {
        size_t idx = vecgrid->__base._size.x * pos.y + pos.x;
        return (((const struct ntg_cell*)vecgrid->__base._data))[idx];
    }
    else
    {
        return ntg_cell_default();
    }
}

/* Writes a rendered cell at `pos`. A `NULL` grid or an out-of-bounds position
 * is ignored. */
static inline void
ntg_cell_vecgrid_set(ntg_cell_vecgrid* vecgrid, struct ntg_cell cell, struct ntg_xy pos)
{
    if(!vecgrid)
        return;

    if(ntg_xy_is_lesser(pos, vecgrid->__base._size))
    {
        size_t idx = vecgrid->__base._size.x * pos.y + pos.x;
        ((struct ntg_cell*)vecgrid->__base._data)[idx] = cell;
    }
}

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
    ntg_vcell_type type;
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
            struct nt_color fg;
            struct nt_style style;
        } overlay;

        struct 
        {
            size_t placeholder;
        } transparent;
    };
};

/* Creates a virtual cell of `type`. `cp` is the code point; full cells copy all
 * of `gfx`, overlay cells copy only its foreground and style, and transparent
 * cells ignore both values.
 *
 * RETURN VALUE:
 * The constructed virtual cell. */
static inline struct ntg_vcell
ntg_vcell_new(ntg_vcell_type type, struct nt_gfx gfx, uint32_t cp)
{
    struct ntg_vcell rval = {0};
    rval.type = type;
    if(type == NTG_VCELL_FULL)
    {
        rval.full.gfx = gfx;
        rval.full.cp = cp;
    }
    else if(type == NTG_VCELL_OVERLAY)
    {
        rval.overlay.fg = gfx.fg;
        rval.overlay.style = gfx.style;
        rval.overlay.cp = cp;
    }

    return rval;
}

/* Creates the default virtual cell: a full space character with default
 * graphics.
 *
 * RETURN VALUE:
 * The default virtual cell. */
static inline struct ntg_vcell 
ntg_vcell_default()
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .full = {
            .cp = ' ',
            .gfx = NT_GFX_DEFAULT
        }
    };
}

/* Creates a full virtual cell whose `cp` and `gfx` replace the character and
 * all graphics of the cell below it.
 *
 * RETURN VALUE:
 * The constructed full virtual cell. */
static inline struct ntg_vcell 
ntg_vcell_full(uint32_t cp, struct nt_gfx gfx)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .full = {
            .cp = cp,
            .gfx = gfx
        }
    };
}

/* Creates an overlay virtual cell whose `cp`, `fg`, and `style` replace the
 * corresponding fields while preserving the background color of the cell below
 * it.
 *
 * RETURN VALUE:
 * The constructed overlay virtual cell. */
static inline struct ntg_vcell 
ntg_vcell_overlay(uint32_t cp, struct nt_color fg, struct nt_style style)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_OVERLAY,
        .overlay = {
            .cp = cp,
            .fg = fg,
            .style = style
        }
    };
}

/* Creates a transparent virtual cell that leaves the cell below it unchanged.
 *
 * RETURN VALUE:
 * The transparent virtual cell. */
static inline struct ntg_vcell 
ntg_vcell_transparent()
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_TRANSPARENT
    };
}

/* Creates a full space cell with the supplied background color and default
 * foreground and style.
 *
 * RETURN VALUE:
 * The constructed background virtual cell. */
static inline struct ntg_vcell 
ntg_vcell_bg(struct nt_color color)
{
    return (struct ntg_vcell) {
        .type = NTG_VCELL_FULL,
        .full = {
            .cp = ' ',
            .gfx = {
                .bg = color,
                .fg = NT_COLOR_DEFAULT,
                .style = NT_STYLE_DEFAULT
            }
        }
    };
}

/* Compares two virtual cells by type and by the fields relevant to that
 * type. */
static inline bool
ntg_vcell_are_equal(struct ntg_vcell c1, struct ntg_vcell c2)
{
    if(c1.type != c2.type) return false;

    if(c1.type == NTG_VCELL_FULL)
    {
        return ((c1.full.cp == c2.full.cp) &&
            nt_gfx_are_equal(c1.full.gfx, c2.full.gfx));
    }
    else if(c1.type == NTG_VCELL_OVERLAY)
    {
        return ((c1.overlay.cp == c2.overlay.cp) &&
            nt_color_are_equal(c1.overlay.fg, c2.overlay.fg) &&
            nt_style_are_equal(c1.overlay.style, c2.overlay.style));
    }
    else return true;
}

/* Composites `overwriting` over `overwritten` according to the virtual-cell
 * type.
 *
 * RETURN VALUE:
 * The resulting rendered cell. A transparent virtual cell returns `overwritten`
 * unchanged. */
static inline struct ntg_cell
ntg_vcell_overwrite(struct ntg_vcell overwriting, struct ntg_cell overwritten)
{
    if(overwriting.type == NTG_VCELL_FULL)
    {
        overwritten.cp = overwriting.full.cp;
        overwritten.gfx = overwriting.full.gfx;
    }
    else if(overwriting.type == NTG_VCELL_OVERLAY)
    {
        overwritten.cp = overwriting.overlay.cp;
        overwritten.gfx.fg = overwriting.overlay.fg;
        overwritten.gfx.style = overwriting.overlay.style;
    }
    else {} // NTG_VCELL_TRANSPARENT

    return overwritten;
}

/* ------------------------------------------------------ */
/* VCELL VECGRID */
/* ------------------------------------------------------ */

struct ntg_vcell_vecgrid
{
    struct ntg_vecgrid __base; 
};

/* Initializes an empty virtual-cell grid. No cell storage is allocated until
 * the grid is resized. */
NTG_API void
ntg_vcell_vecgrid_init(ntg_vcell_vecgrid* vecgrid);
/* Releases storage owned by a virtual-cell grid and resets its base grid.
 * Passing `NULL` has no effect. */
NTG_API void
ntg_vcell_vecgrid_deinit(ntg_vcell_vecgrid* vecgrid);

/* Resizes a virtual-cell grid, preserves overlapping cells, and fills newly
 * added cells with the default full-space virtual cell. `size_cap` caps only
 * extra reserved capacity, not the requested dimensions.
 *
 * ERROR CODES:
 * - `NTG_ERR_ALLOC_FAIL`: `vecgrid` is `NULL` or storage allocation fails.
 * - `NTG_ERR_UNEXPECTED`: the underlying resize fails for another reason,
 *   including arithmetic overflow. */
NTG_API void
ntg_vcell_vecgrid_set_size(
        ntg_vcell_vecgrid* vecgrid,
        struct ntg_xy size,
        struct ntg_xy size_cap,
        int* out_status);
/* Gets the current dimensions of a virtual-cell grid.
 *
 * RETURN VALUE:
 * The grid size, or `(0, 0)` when `vecgrid` is `NULL`. */
NTG_API struct ntg_xy
ntg_vcell_vecgrid_get_size(const ntg_vcell_vecgrid* vecgrid);

/* Reads a virtual cell at `pos` without modifying the grid.
 *
 * RETURN VALUE:
 * The stored virtual cell, or the default full-space virtual cell when the grid
 * is `NULL` or `pos` is outside it. */
static inline struct ntg_vcell
ntg_vcell_vecgrid_get(const ntg_vcell_vecgrid* vecgrid, struct ntg_xy pos)
{
    if(!vecgrid)
        return ntg_vcell_default();

    if(ntg_xy_is_lesser(pos, vecgrid->__base._size))
    {
        size_t idx = vecgrid->__base._size.x * pos.y + pos.x;
        return (((const struct ntg_vcell*)vecgrid->__base._data))[idx];
    }
    else
    {
        return ntg_vcell_default();
    }
}

/* Writes a virtual cell at `pos`. A `NULL` grid or an out-of-bounds position is
 * ignored. */
static inline void
ntg_vcell_vecgrid_set(ntg_vcell_vecgrid* vecgrid, struct ntg_vcell cell, struct ntg_xy pos)
{
    if(!vecgrid) return;

    if(ntg_xy_is_lesser(pos, vecgrid->__base._size))
    {
        size_t idx = vecgrid->__base._size.x * pos.y + pos.x;
        ((struct ntg_vcell*)vecgrid->__base._data)[idx] = cell;
    }
}

#endif // NTG_CELL_H
