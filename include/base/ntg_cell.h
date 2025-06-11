#ifndef _NTG_CELL_H_
#define _NTG_CELL_H_

#include <stdint.h>
#include "shared/ntg_xy.h"
#include "nt_gfx.h"

#define NTG_CELL_EMPTY ' '

/* -------------------------------------------------------------------------- */

struct ntg_rcell
{
    uint32_t codepoint;
    struct nt_gfx gfx;
};

typedef struct ntg_rcell_grid
{
    struct ntg_rcell* __data;
    struct ntg_xy _size;
} ntg_rcell_grid_t;

void __ntg_rcell_grid_init__(ntg_rcell_grid_t* grid);
void __ntg_rcell_grid_deinit__(ntg_rcell_grid_t* grid);

ntg_rcell_grid_t* ntg_rcell_grid_new();
void ntg_rcell_grid_destroy();

#define ntg_rcell_grid_at(grid, pos)                                           \
    (NTG_XY_POS_IN_BOUNDS(pos, grid->_size) ?                                  \
     __data[grid->_size.x * pos.y + pos.x] :                                   \
     NULL)                                                                     \

void ntg_rcell_grid_set_size(ntg_rcell_grid_t* grid, struct ntg_xy size);

/* ------------------------------------------------------ */

typedef enum ntg_cell_type 
{ 
    NTG_CELL_TYPE_REGULAR,
    NTG_CELL_TYPE_TRANSPARENT,
    NTG_CELL_TYPE_OVERLAY 
} ntg_cell_type_t;

typedef struct ntg_cell
{
    struct ntg_rcell __base;
    ntg_cell_type_t _type;
} ntg_cell_t;

typedef struct ntg_cell_grid
{
    struct ntg_cell* __data;
    struct ntg_xy _size;
} ntg_cell_grid_t;

void __ntg_cell_grid_init__(ntg_cell_grid_t* grid);
void __ntg_cell_grid_deinit__(ntg_cell_grid_t* grid);

ntg_rcell_grid_t* ntg_cell_grid_new();
void ntg_cell_grid_destroy();

#define ntg_cell_grid_at(grid, pos)                                            \
    (NTG_XY_POS_IN_BOUNDS(pos, grid->_size) ?                                  \
     __data[grid->_size.x * pos.y + pos.x] :                                   \
     NULL)                                                                     \

void ntg_cell_grid_set_size(ntg_cell_grid_t* grid, struct ntg_xy size);

/* -------------------------------------------------------------------------- */

#endif // _NTG_CELL_H_
