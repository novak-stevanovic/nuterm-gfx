#ifndef _NTG_CELL_H_
#define _NTG_CELL_H_

#include <stdint.h>
#include <string.h>
#include "shared/ntg_xy.h"
#include "nt_gfx.h"
#include "shared/ntg_status.h"

#define NTG_CELL_EMPTY ' '

/* -------------------------------------------------------------------------- */
/* NTG_RCELL */
/* -------------------------------------------------------------------------- */

struct ntg_rcell
{
    uint32_t codepoint;
    struct nt_gfx gfx;
};

static inline struct ntg_rcell ntg_rcell(uint32_t codepoint, struct nt_gfx gfx)
{
    return (struct ntg_rcell) { 
        .codepoint = codepoint,
        .gfx = gfx
    };
}

static inline struct ntg_rcell ntg_rcell_default()
{
    return (struct ntg_rcell) {
        .codepoint = NTG_CELL_EMPTY,
        .gfx = NT_GFX_DEFAULT
    };
}

static inline bool ntg_rcell_are_equal(struct ntg_rcell cell1,
        struct ntg_rcell cell2)
{
    return (memcmp(&cell1, &cell2, sizeof(struct ntg_rcell)) == 0);
}

/* -------------------------------------------------------------------------- */
/* NTG_CELL */
/* -------------------------------------------------------------------------- */

typedef enum ntg_cell_type
{ 
    NTG_CELL_TYPE_FULL,
    NTG_CELL_TYPE_TRANSPARENT,
    NTG_CELL_TYPE_OVERLAY 
} ntg_cell_type;

typedef struct ntg_cell
{
    struct ntg_rcell __base;
    ntg_cell_type _type;
} ntg_cell;

static inline ntg_cell ntg_cell_full(uint32_t codepoint, nt_color fg,
        nt_color bg, nt_style style)
{
    return (ntg_cell) {
        .__base = { codepoint, { fg, bg, style } },
        ._type = NTG_CELL_TYPE_FULL
    };
}

static inline ntg_cell ntg_cell_default()
{
    return (ntg_cell) {
        .__base = ntg_rcell_default(),
        ._type = NTG_CELL_TYPE_FULL
    };
}

static inline ntg_cell ntg_cell_bg(nt_color bg_color)
{
    struct nt_gfx gfx = {
        .fg = NT_COLOR_DEFAULT,
        .bg = bg_color,
        .style = NT_STYLE_DEFAULT
    };

    return (ntg_cell) {
        .__base = ntg_rcell(NTG_CELL_EMPTY, gfx),
        ._type = NTG_CELL_TYPE_FULL
    };
}

static inline ntg_cell ntg_cell_overlay(uint32_t codepoint, nt_color fg,
        nt_style style)
{
    return (ntg_cell) {
        .__base = { .codepoint = codepoint, { fg, NT_COLOR_DEFAULT, style } },
        ._type = NTG_CELL_TYPE_OVERLAY
    };
}

static inline ntg_cell ntg_cell_transparent()
{
    return (ntg_cell) {
        .__base = { NTG_CELL_EMPTY, NT_GFX_DEFAULT },
        ._type = NTG_CELL_TYPE_OVERLAY
    };
}

struct ntg_rcell ntg_cell_overwrite(ntg_cell overwriting,
        struct ntg_rcell overwritten);

static inline bool ntg_cell_are_equal(ntg_cell cell1, ntg_cell cell2)
{
    return (memcmp(&cell1, &cell2, sizeof(struct ntg_rcell)) == 0);
}

/* -------------------------------------------------------------------------- */
/* NTG_RCELL_GRID */
/* -------------------------------------------------------------------------- */

typedef struct ntg_rcell_grid
{
    struct ntg_rcell* __data;
    struct ntg_xy _size;
} ntg_rcell_grid;

void __ntg_rcell_grid_init__(ntg_rcell_grid* grid);
void __ntg_rcell_grid_deinit__(ntg_rcell_grid* grid);

ntg_rcell_grid* ntg_rcell_grid_new();
void ntg_rcell_grid_destroy(ntg_rcell_grid* grid);

void ntg_rcell_grid_set_size(ntg_rcell_grid* grid, struct ntg_xy size,
        ntg_status* out_status);

static inline const struct ntg_rcell* ntg_rcell_grid_at(
        const ntg_rcell_grid* grid, struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

static inline struct ntg_rcell* ntg_rcell_grid_at_(
        ntg_rcell_grid* grid, struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

/* -------------------------------------------------------------------------- */
/* NTG_CELL_GRID */
/* -------------------------------------------------------------------------- */

typedef struct ntg_cell_grid
{
    struct ntg_cell* __data;
    struct ntg_xy _size;
} ntg_cell_grid;

void __ntg_cell_grid_init__(ntg_cell_grid* grid);
void __ntg_cell_grid_deinit__(ntg_cell_grid* grid);

ntg_cell_grid* ntg_cell_grid_new();
void ntg_cell_grid_destroy(ntg_cell_grid* grid);

static inline const ntg_cell* ntg_cell_grid_at(const ntg_cell_grid* grid,
        struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

static inline ntg_cell* ntg_cell_grid_at_(ntg_cell_grid* grid,
        struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

void ntg_cell_grid_set_size(ntg_cell_grid* grid, struct ntg_xy size,
        ntg_status* out_status);

/* -------------------------------------------------------------------------- */
/* NTG_RCELL_VGRID */
/* -------------------------------------------------------------------------- */

typedef struct ntg_rcell_vgrid
{
    ntg_rcell_grid ___data; // grid + allocated size
    struct ntg_xy __size; // current size
} ntg_rcell_vgrid;

void __ntg_rcell_vgrid_init__(ntg_rcell_vgrid* vgrid);
void __ntg_rcell_vgrid_deinit__(ntg_rcell_vgrid* vgrid);

ntg_rcell_vgrid* ntg_rcell_vgrid_new();
void ntg_rcell_vgrid_destroy(ntg_rcell_vgrid* vgrid);

void ntg_rcell_vgrid_set_size(ntg_rcell_vgrid* vgrid, struct ntg_xy size,
        ntg_status* out_status);

struct ntg_xy ntg_rcell_vgrid_get_size(const ntg_rcell_vgrid* vgrid);

static inline const struct ntg_rcell* ntg_rcell_vgrid_at(
        const ntg_rcell_vgrid* vgrid, struct ntg_xy pos)
{
    return ((vgrid != NULL) ?
            ntg_rcell_grid_at(&vgrid->___data, pos) :
            NULL);
}

static inline struct ntg_rcell* ntg_rcell_vgrid_at_(
        ntg_rcell_vgrid* vgrid, struct ntg_xy pos)
{
    return ((vgrid != NULL) ?
            ntg_rcell_grid_at_(&vgrid->___data, pos) :
            NULL);
}

/* -------------------------------------------------------------------------- */
/* NTG_CELL_VGRID */
/* -------------------------------------------------------------------------- */

typedef struct ntg_cell_vgrid
{
    ntg_cell_grid ___data; // grid + allocated size
    struct ntg_xy __size; // current size
} ntg_cell_vgrid;

void __ntg_cell_vgrid_init__(ntg_cell_vgrid* vgrid);
void __ntg_cell_vgrid_deinit__(ntg_cell_vgrid* vgrid);

ntg_cell_vgrid* ntg_cell_vgrid_new();
void ntg_cell_vgrid_destroy(ntg_cell_vgrid* vgrid);

void ntg_cell_vgrid_set_size(ntg_cell_vgrid* vgrid, struct ntg_xy size,
        ntg_status* out_status);

struct ntg_xy ntg_cell_vgrid_get_size(const ntg_cell_vgrid* vgrid);

static inline const struct ntg_cell* ntg_cell_vgrid_at(
        const ntg_cell_vgrid* vgrid, struct ntg_xy pos)
{
    return ((vgrid != NULL) ?
            ntg_cell_grid_at(&vgrid->___data, pos) :
            NULL);
}

static inline struct ntg_cell* ntg_cell_vgrid_at_(
        ntg_cell_vgrid* vgrid, struct ntg_xy pos)
{
    return ((vgrid != NULL) ?
            ntg_cell_grid_at_(&vgrid->___data, pos) :
            NULL);
}

#endif // _NTG_CELL_H_
