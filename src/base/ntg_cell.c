#include <stdlib.h>
#include <assert.h>

#include "base/ntg_cell.h"
#include "shared/_ntg_shared.h"

void __ntg_rcell_grid_init__(ntg_rcell_grid_t* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

void __ntg_rcell_grid_deinit__(ntg_rcell_grid_t* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

ntg_rcell_grid_t* ntg_rcell_grid_new()
{
    ntg_rcell_grid_t* new = (ntg_rcell_grid_t*)malloc(
            sizeof(struct ntg_rcell_grid));

    if(new == NULL) return NULL;

    __ntg_rcell_grid_init__(new);

    return new;
}

void ntg_rcell_grid_destroy(ntg_rcell_grid_t* grid)
{
    if(grid == NULL) return;

    __ntg_rcell_grid_deinit__(grid);

    free(grid);
}

void ntg_rcell_grid_set_size(ntg_rcell_grid_t* grid, struct ntg_xy size,
        ntg_status_t* out_status)
{
    if(grid == NULL)
        _vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(ntg_xy_are_equal(size, ntg_xy(0, 0)))
    {
        grid->_size = size;
        if(grid->__data != NULL)
            free(grid->__data);
    }
    else
    {
        void* new_data = realloc(grid->__data,
                size.x * size.y * sizeof(struct ntg_rcell));

        if(new_data == NULL)
            _vreturn(out_status, NTG_ERR_ALLOC_FAIL);

        grid->__data = new_data;
        grid->_size = size;
    }

    _vreturn(out_status, NTG_SUCCESS);
}

/* ------------------------------------------------------ */

struct ntg_rcell ntg_cell_overwrite(ntg_cell_t overwriting,
        struct ntg_rcell overwritten)
{
    switch(overwriting._type)
    {
        case NTG_CELL_TYPE_FULL:

            overwritten = (struct ntg_rcell) {
                .codepoint = overwriting.__base.codepoint,
                .gfx = {
                    .fg = overwriting.__base.gfx.fg,
                    .bg = overwriting.__base.gfx.bg,
                    .style = overwriting.__base.gfx.style
                }
            };
            break;

        case NTG_CELL_TYPE_OVERLAY:

            overwritten = (struct ntg_rcell) {
                .codepoint = overwriting.__base.codepoint,
                .gfx = {
                    .fg = overwriting.__base.gfx.fg,
                    .bg = overwritten.gfx.bg,
                    .style = overwriting.__base.gfx.style
                }
            };
            break;

        case NTG_CELL_TYPE_TRANSPARENT:
            break;
    }

    return overwritten;
}

/* ------------------------------------------------------ */

void __ntg_cell_grid_init__(ntg_cell_grid_t* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

void __ntg_cell_grid_deinit__(ntg_cell_grid_t* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

ntg_cell_grid_t* ntg_cell_grid_new()
{
    ntg_cell_grid_t* new = (ntg_cell_grid_t*)malloc(
            sizeof(struct ntg_cell_grid));

    if(new == NULL) return NULL;

    __ntg_cell_grid_init__(new);

    return new;
}

void ntg_cell_grid_destroy(ntg_cell_grid_t* grid)
{
    if(grid == NULL) return;

    __ntg_cell_grid_deinit__(grid);

    free(grid);
}

void ntg_cell_grid_set_size(ntg_cell_grid_t* grid, struct ntg_xy size,
        ntg_status_t* out_status)
{
    if(grid == NULL)
        _vreturn(out_status, NTG_ERR_INVALID_ARG);

    if(ntg_xy_are_equal(size, ntg_xy(0, 0)))
    {
        grid->_size = size;
        if(grid->__data != NULL)
            free(grid->__data);
    }
    else
    {
        void* new_data = realloc(grid->__data,
                size.x * size.y * sizeof(struct ntg_cell));

        if(new_data == NULL)
            _vreturn(out_status, NTG_ERR_ALLOC_FAIL);

        grid->__data = new_data;
        grid->_size = size;
    }

    _vreturn(out_status, NTG_SUCCESS);
}
