#ifndef _NTG_CELL_H_
#define _NTG_CELL_H_

#include <stdint.h>
#include <string.h>
#include "shared/ntg_typedef.h"
#include "nt_gfx.h"
#include "base/ntg_xy.h"

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
    return (memcmp(&c1, &c2, sizeof(struct ntg_cell)) == 0);
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
    ntg_vcell_type type;
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
    return (memcmp(&c1, &c2, sizeof(struct ntg_cell)) == 0);
}

struct ntg_cell ntg_vcell_overwrite(
        struct ntg_vcell overwriting,
        struct ntg_cell overwritten);

/* -------------------------------------------------------------------------- */
/* CELL GRID */
/* -------------------------------------------------------------------------- */

struct ntg_cell_grid
{
    struct ntg_cell* __data;
    struct ntg_xy _size;
};

void ntg_cell_grid_init(ntg_cell_grid* grid);
void ntg_cell_grid_deinit(ntg_cell_grid* grid);

void ntg_cell_grid_set_size(ntg_cell_grid* grid,
        struct ntg_xy size, ntg_status* out_status);

static inline const struct ntg_cell* ntg_cell_grid_at(
        const ntg_cell_grid* grid, struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

static inline struct ntg_cell* ntg_cell_grid_at_(
        ntg_cell_grid* grid, struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

/* -------------------------------------------------------------------------- */
/* VCELL GRID */
/* -------------------------------------------------------------------------- */

struct ntg_vcell_grid
{
    struct ntg_vcell* __data;
    struct ntg_xy _size;
};

void ntg_vcell_grid_init(ntg_vcell_grid* grid);
void ntg_vcell_grid_deinit(ntg_vcell_grid* grid);

static inline const struct ntg_vcell* ntg_vcell_grid_at(
        const ntg_vcell_grid* grid, struct ntg_xy pos)
{
    size_t idx = pos.y * grid->_size.x + pos.x;

    return ((grid != NULL) ? &(grid->__data[idx]) : NULL);
}

static inline struct ntg_vcell* ntg_vcell_grid_at_(
        ntg_vcell_grid* grid, struct ntg_xy pos)
{
    size_t idx = pos.y * grid->_size.x + pos.x;

    return ((grid != NULL) ? &(grid->__data[idx]) : NULL);
}

void ntg_vcell_grid_set_size(ntg_vcell_grid* grid,
        struct ntg_xy size, ntg_status* out_status);

/* -------------------------------------------------------------------------- */
/* CELL VECGRID */
/* -------------------------------------------------------------------------- */

struct ntg_cell_vecgrid
{
    ntg_cell_grid __data; // grid + allocated size
    struct ntg_xy __size; // current size
};

void ntg_cell_vecgrid_init(ntg_cell_vecgrid* vecgrid);
void ntg_cell_vecgrid_deinit(ntg_cell_vecgrid* vecgrid);

void ntg_cell_vecgrid_set_size(ntg_cell_vecgrid* vecgrid, struct ntg_xy size,
        ntg_status* out_status);

struct ntg_xy ntg_cell_vecgrid_get_size(const ntg_cell_vecgrid* vecgrid);

static inline const struct ntg_cell* ntg_cell_vecgrid_at(
        const ntg_cell_vecgrid* vecgrid, struct ntg_xy pos)
{
    return ((vecgrid != NULL) ?
            ntg_cell_grid_at(&vecgrid->__data, pos) :
            NULL);
}

static inline struct ntg_cell* ntg_cell_vecgrid_at_(
        ntg_cell_vecgrid* vecgrid, struct ntg_xy pos)
{
    return ((vecgrid != NULL) ?
            ntg_cell_grid_at_(&vecgrid->__data, pos) :
            NULL);
}

/* -------------------------------------------------------------------------- */
/* VCELL VECGRID */
/* -------------------------------------------------------------------------- */

struct ntg_vcell_vecgrid
{
    ntg_vcell_grid __data; // grid + allocated size
    struct ntg_xy __size; // current size
};

void ntg_vcell_vecgrid_init(ntg_vcell_vecgrid* vecgrid);
void ntg_vcell_vecgrid_deinit(ntg_vcell_vecgrid* vecgrid);

void ntg_vcell_vecgrid_set_size(ntg_vcell_vecgrid* vecgrid,
        struct ntg_xy size, ntg_status* out_status);

struct ntg_xy ntg_vcell_vecgrid_get_size(const ntg_vcell_vecgrid* vecgrid);

static inline const struct ntg_vcell* ntg_vcell_vecgrid_at(
        const ntg_vcell_vecgrid* vecgrid, struct ntg_xy pos)
{
    return ((vecgrid != NULL) ?
            ntg_vcell_grid_at(&vecgrid->__data, pos) :
            NULL);
}

static inline struct ntg_vcell* ntg_vcell_vecgrid_at_(
        ntg_vcell_vecgrid* vecgrid, struct ntg_xy pos)
{
    return ((vecgrid != NULL) ?
            ntg_vcell_grid_at_(&vecgrid->__data, pos) :
            NULL);
}

#endif // _NTG_CELL_H_
