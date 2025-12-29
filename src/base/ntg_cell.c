#include <stdlib.h>
#include <assert.h>

#include "base/ntg_cell.h"
#include "shared/_ntg_shared.h"

struct ntg_cell ntg_vcell_overwrite(
        struct ntg_vcell overwriting,
        struct ntg_cell overwritten)
{
    switch(overwriting.type)
    {
        case NTG_VCELL_FULL:
            overwritten.codepoint = overwriting.full.codepoint;
            overwritten.gfx = overwriting.full.gfx;

            break;

        case NTG_VCELL_OVERLAY:
            overwritten.codepoint = overwriting.overlay.codepoint;
            overwritten.gfx.fg = overwriting.overlay.fg;
            overwritten.gfx.style = overwriting.overlay.style;
            break;

        case NTG_VCELL_TRANSPARENT:
            break;
    }

    return overwritten;
}

/* ------------------------------------------------------ */

void _ntg_cell_grid_init_(ntg_cell_grid* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

void _ntg_cell_grid_deinit_(ntg_cell_grid* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    free(grid->__data);
    grid->__data = NULL;
}

void ntg_cell_grid_set_size(ntg_cell_grid* grid,
        struct ntg_xy size, ntg_status* out_status)
{
    if(grid == NULL)
        _vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_xy_size_(&size);

    if((size.x == 0) && (size.y == 0))
    {
        grid->_size = size;
        if(grid->__data != NULL)
        {
            free(grid->__data);
            grid->__data = NULL;
        }
    }
    else
    {
        void* new_data = realloc(grid->__data,
                size.x * size.y * sizeof(struct ntg_cell));

        if(new_data == NULL)
            _vreturn(out_status, NTG_ERR_ALLOC_FAIL);

        struct ntg_xy old_size = grid->_size;

        grid->__data = new_data;
        grid->_size = size;

        size_t i, j;
        struct ntg_cell* it_cell;
        for(i = 0; i < size.y; i++)
        {
            for(j = 0; j < size.x; j++)
            {
                if((i >= old_size.y) || (j >= old_size.x))
                {
                    it_cell = ntg_cell_grid_at_(grid, ntg_xy(j, i));
                    (*it_cell) = ntg_cell_default();
                }
            }
        }
    }

    _vreturn(out_status, NTG_SUCCESS);
}

/* -------------------------------------------------------------------------- */
/* VCELL GRID */
/* -------------------------------------------------------------------------- */

void _ntg_vcell_grid_init_(ntg_vcell_grid* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

void _ntg_vcell_grid_deinit_(ntg_vcell_grid* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    free(grid->__data);
    grid->__data = NULL;
}

void ntg_vcell_grid_set_size(ntg_vcell_grid* grid,
        struct ntg_xy size, ntg_status* out_status)
{
    if(grid == NULL)
        _vreturn(out_status, NTG_ERR_INVALID_ARG);

    ntg_xy_size_(&size);

    if((size.x == 0) && (size.y == 0))
    {
        grid->_size = size;
        if(grid->__data != NULL)
        {
            free(grid->__data);
            grid->__data = NULL;
        }
    }
    else
    {
        void* new_data = realloc(grid->__data,
                size.x * size.y * sizeof(struct ntg_vcell));

        if(new_data == NULL)
            _vreturn(out_status, NTG_ERR_ALLOC_FAIL);

        struct ntg_xy old_size = grid->_size;

        grid->__data = new_data;
        grid->_size = size;

        size_t i, j;
        struct ntg_vcell* it_cell;
        for(i = 0; i < size.y; i++)
        {
            for(j = 0; j < size.x; j++)
            {
                if((i >= old_size.y) || (j >= old_size.x))
                {
                    it_cell = ntg_vcell_grid_at_(grid, ntg_xy(j, i));
                    (*it_cell) = ntg_vcell_default();
                }
            }
        }
    }

    _vreturn(out_status, NTG_SUCCESS);
}

/* -------------------------------------------------------------------------- */
/* CELL VECGRID */
/* -------------------------------------------------------------------------- */

void _ntg_cell_vecgrid_init_(ntg_cell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    _ntg_cell_grid_init_(&vecgrid->__data);

    vecgrid->__size = NTG_XY_UNSET;
}

void _ntg_cell_vecgrid_deinit_(ntg_cell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    _ntg_cell_grid_deinit_(&vecgrid->__data);

    vecgrid->__size = NTG_XY_UNSET;
}

struct ntg_xy ntg_cell_vecgrid_get_size(const ntg_cell_vecgrid* vecgrid)
{
    return vecgrid->__size;
}

void ntg_cell_vecgrid_set_size(ntg_cell_vecgrid* vecgrid, struct ntg_xy size,
        ntg_status* out_status)
{
    ntg_xy_size_(&size);

    ntg_cell_grid_set_size(&vecgrid->__data, size, NULL);

    vecgrid->__size = size;

    _vreturn(out_status, NTG_SUCCESS);
}

/* -------------------------------------------------------------------------- */
/* VCELL VECGRID */
/* -------------------------------------------------------------------------- */

void _ntg_vcell_vecgrid_init_(ntg_vcell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    _ntg_vcell_grid_init_(&vecgrid->__data);

    vecgrid->__size = NTG_XY_UNSET;
}

void _ntg_vcell_vecgrid_deinit_(ntg_vcell_vecgrid* vecgrid)
{
    assert(vecgrid != NULL);

    _ntg_vcell_grid_deinit_(&vecgrid->__data);

    vecgrid->__size = NTG_XY_UNSET;
}

struct ntg_xy ntg_vcell_vecgrid_get_size(const ntg_vcell_vecgrid* vecgrid)
{
    return vecgrid->__size;
}

void ntg_vcell_vecgrid_set_size(ntg_vcell_vecgrid* vecgrid, struct ntg_xy size,
        ntg_status* out_status)
{
    ntg_xy_size_(&size);

    ntg_vcell_grid_set_size(&vecgrid->__data, size, NULL);

    vecgrid->__size = size;

    _vreturn(out_status, NTG_SUCCESS);
}
