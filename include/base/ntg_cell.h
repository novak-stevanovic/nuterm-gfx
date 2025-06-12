#ifndef _NTG_CELL_H_
#define _NTG_CELL_H_

#include <stdint.h>
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

/* -------------------------------------------------------------------------- */
/* NTG_RCELL_GRID */
/* -------------------------------------------------------------------------- */

typedef struct ntg_rcell_grid
{
    struct ntg_rcell* __data;
    struct ntg_xy _size;
} ntg_rcell_grid_t;

void __ntg_rcell_grid_init__(ntg_rcell_grid_t* grid);
void __ntg_rcell_grid_deinit__(ntg_rcell_grid_t* grid);

ntg_rcell_grid_t* ntg_rcell_grid_new();
void ntg_rcell_grid_destroy(ntg_rcell_grid_t* grid);

void ntg_rcell_grid_set_size(ntg_rcell_grid_t* grid, struct ntg_xy size,
        ntg_status_t* out_status);

static inline const struct ntg_rcell* ntg_rcell_grid_at(
        const ntg_rcell_grid_t* grid, struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

static inline struct ntg_rcell* ntg_rcell_grid_at_(
        ntg_rcell_grid_t* grid, struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

/* -------------------------------------------------------------------------- */
/* NTG_CELL_TYPE */
/* -------------------------------------------------------------------------- */

typedef enum ntg_cell_type 
{ 
    NTG_CELL_TYPE_FULL,
    NTG_CELL_TYPE_TRANSPARENT,
    NTG_CELL_TYPE_OVERLAY 
} ntg_cell_type_t;

typedef struct ntg_cell
{
    struct ntg_rcell __base;
    ntg_cell_type_t _type;
} ntg_cell_t;

static inline ntg_cell_t ntg_cell_full(uint32_t codepoint, nt_color_t fg,
        nt_color_t bg, nt_style_t style)
{
    return (ntg_cell_t) {
        .__base = { codepoint, { fg, bg, style } },
        ._type = NTG_CELL_TYPE_FULL
    };
}

static inline ntg_cell_t ntg_cell_default()
{
    return (ntg_cell_t) {
        .__base = ntg_rcell_default(),
        ._type = NTG_CELL_TYPE_FULL
    };
}

static inline ntg_cell_t ntg_cell_overlay(uint32_t codepoint, nt_color_t fg,
        nt_style_t style)
{
    return (ntg_cell_t) {
        .__base = { .codepoint = codepoint, { fg, NT_COLOR_DEFAULT, style } },
        ._type = NTG_CELL_TYPE_OVERLAY
    };
}

static inline ntg_cell_t ntg_cell_transparent()
{
    return (ntg_cell_t) {
        .__base = { NTG_CELL_EMPTY, NT_GFX_DEFAULT },
        ._type = NTG_CELL_TYPE_OVERLAY
    };
}

struct ntg_rcell ntg_cell_overwrite(ntg_cell_t overwriting,
        struct ntg_rcell overwritten);

/* -------------------------------------------------------------------------- */
/* NTG_CELL_GRID */
/* -------------------------------------------------------------------------- */

typedef struct ntg_cell_grid
{
    struct ntg_cell* __data;
    struct ntg_xy _size;
} ntg_cell_grid_t;

void __ntg_cell_grid_init__(ntg_cell_grid_t* grid);
void __ntg_cell_grid_deinit__(ntg_cell_grid_t* grid);

ntg_cell_grid_t* ntg_cell_grid_new();
void ntg_cell_grid_destroy(ntg_cell_grid_t* grid);

static inline const ntg_cell_t* ntg_cell_grid_at(const ntg_cell_grid_t* grid,
        struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

static inline ntg_cell_t* ntg_cell_grid_at_(ntg_cell_grid_t* grid,
        struct ntg_xy pos)
{
    return ((grid != NULL) ?
            &(grid->__data[pos.y * grid->_size.x + pos.x]) :
            NULL);
}

void ntg_cell_grid_set_size(ntg_cell_grid_t* grid, struct ntg_xy size,
        ntg_status_t* out_status);

#endif // _NTG_CELL_H_
