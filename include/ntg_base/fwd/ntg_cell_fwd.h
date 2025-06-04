#ifndef _NTG_CELL_FWD_H_
#define _NTG_CELL_FWD_H_

#include <stdint.h>
#include "nt_gfx.h"

struct ntg_cell_base
{
    uint32_t codepoint;
    struct nt_gfx gfx;
};

typedef struct ntg_cell ntg_cell_t;

typedef struct ntg_cell_grid ntg_cell_grid_t;

typedef struct ntg_cell_base_grid ntg_cell_base_grid_t;

#endif // _NTG_CELL_FWD_H_
