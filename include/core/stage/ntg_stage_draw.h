#ifndef NTG_STAGE_DRAW_H
#define NTG_STAGE_DRAW_H

#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"
#include "base/ntg_cell.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_stage_draw
{
    struct
    {
        ntg_cell_vecgrid data;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_stage_draw_init(ntg_stage_draw* drawing);

NTG_API int
ntg_stage_draw_deinit(ntg_stage_draw* drawing);

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

static inline struct ntg_xy
ntg_stage_draw_get_size(const ntg_stage_draw* drawing)
{
    return (drawing != NULL) ?
        ntg_cell_vecgrid_get_size(&drawing->priv.data) :
        NTG_XY_UNSET;
}

NTG_API int
ntg_stage_draw_set_size(ntg_stage_draw* drawing, struct ntg_xy size);

/* ------------------------------------------------------ */
/* CELLS */
/* ------------------------------------------------------ */

static inline struct ntg_cell
ntg_stage_draw_get(const ntg_stage_draw* drawing, struct ntg_xy pos)
{
    if(!drawing) return ntg_cell_default();

    return ntg_cell_vecgrid_get(&drawing->priv.data, pos);
}


static inline void
ntg_stage_draw_set(ntg_stage_draw* drawing, struct ntg_cell cell, struct ntg_xy pos)
{
    if(!drawing) return;

    ntg_cell_vecgrid_set(&drawing->priv.data, cell, pos);
}

#endif // NTG_STAGE_DRAW_H
