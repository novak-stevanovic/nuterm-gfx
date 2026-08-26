#ifndef NTG_STAGE_DRAW_H
#define NTG_STAGE_DRAW_H

#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"
#include "base/ntg_xy.h"
#include "base/ntg_cell.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

GENC_VECTOR_INLINE(ntg_cell_vec, struct ntg_cell, 1.5)

struct ntg_stage_draw
{
    struct
    {
        struct ntg_cell_vec cell_vec;
    } priv;
    struct
    {
        struct ntg_xy size;
    } ro;
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

NTG_API int
ntg_stage_draw_set_size(ntg_stage_draw* drawing, struct ntg_xy size);

/* ------------------------------------------------------ */
/* CELLS */
/* ------------------------------------------------------ */

static inline struct ntg_cell
ntg_stage_draw_get(const ntg_stage_draw* drawing, struct ntg_xy pos)
{
    if(!drawing) return ntg_cell_default();

    struct ntg_xy size = drawing->ro.size;

    if((pos.x < size.x) && (pos.y < size.y))
        return drawing->priv.cell_vec.data[pos.y * size.x + pos.x];
    else
        return ntg_cell_default();
}

static inline void
ntg_stage_draw_set(ntg_stage_draw* drawing, struct ntg_cell cell, struct ntg_xy pos)
{
    if(!drawing) return;

    struct ntg_xy size = drawing->ro.size;

    if((pos.x < size.x) && (pos.y < size.y))
        drawing->priv.cell_vec.data[pos.y * size.x + pos.x] = cell;
}

#endif // NTG_STAGE_DRAW_H
