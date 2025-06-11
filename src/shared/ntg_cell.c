#include <assert.h>
#include <stdlib.h>

#include "base/ntg_cell.h"

/* -------------------------------------------------------------------------- */

void __ntg_rcell_grid_init__(ntg_rcell_grid_t* grid)
{
    grid->_size = NTG_XY_UNSET;
    grid->__data = NULL;
}

void __ntg_rcell_grid_deinit__(ntg_rcell_grid_t* grid)
{
    grid->_size = NTG_XY_UNSET;

    if(grid->__data != NULL)
        free(grid->__data);
    grid->__data = NULL;
}

ntg_rcell_grid_t* ntg_rcell_grid_new()
{
}

void ntg_rcell_grid_destroy()
{
}

#define ntg_rcell_grid_at(grid, pos)                                           \
    (NTG_XY_POS_IN_BOUNDS(pos, grid->_size) ?                                  \
     __data[grid->_size.x * pos.y + pos.x] :                                   \
     NULL)                                                                     \

void ntg_rcell_grid_set_size(ntg_rcell_grid_t* grid, struct ntg_xy size)
{
}

/* -------------------------------------------------------------------------- */

void ntg_cell(ntg_cell_t* cell, uint32_t codepoint,
        nt_color_t fg, nt_color_t bg, nt_style_t style)
{
    if(cell == NULL) return;

    cell->_type = NTG_CELL_TYPE_REGULAR;
    cell->_base.codepoint = codepoint;
    cell->_base.gfx = (struct nt_gfx) {
        .fg = fg,
        .bg = bg,
        .style = style
    };
}

void ntg_cell_overlay(ntg_cell_t* cell, uint32_t codepoint,
        nt_color_t fg, nt_style_t style)
{
    if(cell == NULL) return;

    cell->_type = NTG_CELL_TYPE_OVERLAY;
    cell->_base.codepoint = codepoint;
    cell->_base.gfx = (struct nt_gfx) {
        .fg = fg,
        .bg = NT_COLOR_DEFAULT,
        .style = style
    };
}

void ntg_cell_transparent(ntg_cell_t* cell)
{
    if(cell == NULL) return;

    cell->_type = NTG_CELL_TYPE_TRANSPARENT;
    cell->_base.codepoint = NTG_CELL_EMPTY;
    cell->_base.gfx = (struct nt_gfx) {
        .fg = NT_COLOR_DEFAULT,
        .bg = NT_COLOR_DEFAULT,
        .style = NT_STYLE_DEFAULT
    };
}

void ntg_cell_default(ntg_cell_t* cell)
{
    if(cell == NULL) return;

    cell->_type = NTG_CELL_TYPE_REGULAR;
    cell->_base.codepoint = NTG_CELL_EMPTY;
    cell->_base.gfx = (struct nt_gfx) {
        .fg = NT_COLOR_DEFAULT,
        .bg = NT_COLOR_DEFAULT,
        .style = NT_STYLE_DEFAULT
    };
}

void ntg_cell_overwrite(const struct ntg_cell* overwriting,
        struct ntg_rcell* overwritten)
{
    if(overwriting == NULL) return;
    if(overwritten == NULL) return;

    switch(overwriting->_type)
    {
        case NTG_CELL_TYPE_REGULAR:
            overwritten->codepoint = overwriting->__base.codepoint;
            overwritten->gfx = overwriting->__base.gfx;
            break;
        case NTG_CELL_TYPE_OVERLAY:
            overwritten->codepoint = overwriting->__base.codepoint;
            overwritten->gfx = (struct nt_gfx) {
                .fg = overwriting->__base.gfx.bg,
                .bg = overwritten->gfx.bg,
                .style = overwriting->__base.gfx.style
            };
            break;
        case NTG_CELL_TYPE_TRANSPARENT:
            break;
    }
}

/* ------------------------------------------------------ */

ntg_cell_grid_t* ntg_cell_grid_new(struct ntg_xy size)
{
    ntg_cell_grid_t* new = (ntg_cell_grid_t*)malloc(sizeof(struct ntg_cell_grid));
    if(new == NULL) return NULL;

    new->data = (struct ntg_cell*)malloc
        (sizeof(struct ntg_cell) * size.x * size.y);

    if(new->data == NULL)
    {
        free(new);
        return NULL;
    }

    new->size = size;

    return new;
}

void ntg_cell_grid_destroy(ntg_cell_grid_t* grid)
{
    if(grid == NULL) return;

    if(grid->data != NULL) free(grid->data);
    free(grid);
}

const struct ntg_cell* ntg_cell_grid_at(const ntg_cell_grid_t* grid,
        struct ntg_xy pos)
{
    if(grid == NULL) return NULL;

    if((pos.x >= grid->size.x) || (pos.y >= grid->size.y))
        return NULL;

    return &grid->data[pos.y * grid->size.x + pos.x];
}

struct ntg_cell* ntg_cell_grid_at_(ntg_cell_grid_t* grid,
        struct ntg_xy pos)
{
    if(grid == NULL) return NULL;

    if((pos.x >= grid->size.x) || (pos.y >= grid->size.y))
        return NULL;

    return &grid->data[pos.y * grid->size.x + pos.x];
}

void ntg_cell_grid_realloc(ntg_cell_grid_t* grid, struct ntg_xy size)
{
    if(grid == NULL) return;

    struct ntg_cell* new_data = realloc
        (grid->data, sizeof(struct ntg_cell) * size.x * size.y);

    if(new_data == NULL)
        return;
    else
    {
        grid->data = new_data;
        grid->size = size;
    }
}

struct ntg_xy ntg_cell_grid_get_size(const ntg_cell_grid_t* grid)
{
    return (grid != NULL) ? grid->size : NTG_XY_UNSET;
}
