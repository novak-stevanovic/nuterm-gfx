#include <stdlib.h>
#include <assert.h>

#include "base/ntg_cell.h"
#include "shared/_ntg_shared.h"
#include "shared/ntg_log.h"

void __ntg_rcell_grid_init__(ntg_rcell_grid* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

void __ntg_rcell_grid_deinit__(ntg_rcell_grid* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

ntg_rcell_grid* ntg_rcell_grid_new()
{
    ntg_rcell_grid* new = (ntg_rcell_grid*)malloc(
            sizeof(struct ntg_rcell_grid));

    if(new == NULL) return NULL;

    __ntg_rcell_grid_init__(new);

    return new;
}

void ntg_rcell_grid_destroy(ntg_rcell_grid* grid)
{
    if(grid == NULL) return;

    __ntg_rcell_grid_deinit__(grid);

    free(grid);
}

void ntg_rcell_grid_set_size(ntg_rcell_grid* grid, struct ntg_xy size,
        ntg_status* out_status)
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
                size.x * size.y * sizeof(struct ntg_rcell));

        if(new_data == NULL)
            _vreturn(out_status, NTG_ERR_ALLOC_FAIL);

        grid->__data = new_data;
        grid->_size = size;
    }

    _vreturn(out_status, NTG_SUCCESS);
}

/* ------------------------------------------------------ */

struct ntg_rcell ntg_cell_overwrite(ntg_cell overwriting,
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

void __ntg_cell_grid_init__(ntg_cell_grid* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

void __ntg_cell_grid_deinit__(ntg_cell_grid* grid)
{
    if(grid == NULL) return;

    grid->_size = ntg_xy(0, 0);
    grid->__data = NULL;
}

ntg_cell_grid* ntg_cell_grid_new()
{
    ntg_cell_grid* new = (ntg_cell_grid*)malloc(
            sizeof(struct ntg_cell_grid));

    if(new == NULL) return NULL;

    __ntg_cell_grid_init__(new);

    return new;
}

void ntg_cell_grid_destroy(ntg_cell_grid* grid)
{
    if(grid == NULL) return;

    __ntg_cell_grid_deinit__(grid);

    free(grid);
}

void ntg_cell_grid_set_size(ntg_cell_grid* grid, struct ntg_xy size,
        ntg_status* out_status)
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

        grid->__data = new_data;
        grid->_size = size;
    }

    _vreturn(out_status, NTG_SUCCESS);
}

/* -------------------------------------------------------------------------- */
/* NTG_RCELL_VGRID */
/* -------------------------------------------------------------------------- */

void __ntg_rcell_vgrid_init__(ntg_rcell_vgrid* vgrid)
{
    assert(vgrid != NULL);

    __ntg_rcell_grid_init__(&vgrid->___data);

    vgrid->__size = NTG_XY_UNSET;
}

void __ntg_rcell_vgrid_deinit__(ntg_rcell_vgrid* vgrid)
{
    assert(vgrid != NULL);

    __ntg_rcell_grid_deinit__(&vgrid->___data);

    vgrid->__size = NTG_XY_UNSET;
}

ntg_rcell_vgrid* ntg_rcell_vgrid_new()
{
    ntg_rcell_vgrid* new = (ntg_rcell_vgrid*)malloc(
            sizeof(struct ntg_rcell_vgrid));

    if(new == NULL) return NULL;

    __ntg_rcell_grid_init__(&new->___data);

    return new;
}

void ntg_rcell_vgrid_destroy(ntg_rcell_vgrid* vgrid)
{
    assert(vgrid != NULL);

    __ntg_rcell_grid_deinit__(&vgrid->___data);
}

struct ntg_xy ntg_rcell_vgrid_get_size(const ntg_rcell_vgrid* vgrid)
{
    return vgrid->__size;
}

void ntg_rcell_vgrid_set_size(ntg_rcell_vgrid* vgrid, struct ntg_xy size,
        ntg_status* out_status)
{
    // TODO

    ntg_xy_size_(&size);

    ntg_rcell_grid_set_size(&vgrid->___data, size, NULL);

    vgrid->__size = size;

    _vreturn(out_status, NTG_SUCCESS);
}

/* -------------------------------------------------------------------------- */
/* NTG_CELL_VGRID */
/* -------------------------------------------------------------------------- */

void __ntg_cell_vgrid_init__(ntg_cell_vgrid* vgrid)
{
    assert(vgrid != NULL);

    __ntg_cell_grid_init__(&vgrid->___data);

    vgrid->__size = NTG_XY_UNSET;
}

void __ntg_cell_vgrid_deinit__(ntg_cell_vgrid* vgrid)
{
    assert(vgrid != NULL);

    __ntg_cell_grid_deinit__(&vgrid->___data);

    vgrid->__size = NTG_XY_UNSET;
}

ntg_cell_vgrid* ntg_cell_vgrid_new()
{
    ntg_cell_vgrid* new = (ntg_cell_vgrid*)malloc(
            sizeof(struct ntg_cell_vgrid));

    if(new == NULL) return NULL;

    __ntg_cell_grid_init__(&new->___data);

    return new;
}

void ntg_cell_vgrid_destroy(ntg_cell_vgrid* vgrid)
{
    assert(vgrid != NULL);

    __ntg_cell_grid_deinit__(&vgrid->___data);
}

struct ntg_xy ntg_cell_vgrid_get_size(const ntg_cell_vgrid* vgrid)
{
    return vgrid->__size;
}

void ntg_cell_vgrid_set_size(ntg_cell_vgrid* vgrid, struct ntg_xy size,
        ntg_status* out_status)
{
    // TODO
    ntg_xy_size_(&size);

    ntg_cell_grid_set_size(&vgrid->___data, size, NULL);

    vgrid->__size = size;

    _vreturn(out_status, NTG_SUCCESS);
}
