#ifndef NTG_OBJECT_DRAW_H
#define NTG_OBJECT_DRAW_H

#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"
#include "base/ntg_cell.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_object_draw
{
    struct
    {
        ntg_vcell_vecgrid data;
    } priv;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_object_draw_init(ntg_object_draw* drawing);

NTG_API int
ntg_object_draw_deinit(ntg_object_draw* drawing);

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

static inline struct ntg_xy
ntg_object_draw_get_size(const ntg_object_draw* drawing)
{
    return (drawing != NULL) ?
        ntg_vcell_vecgrid_get_size(&drawing->priv.data) :
        NTG_XY_UNSET;
}

NTG_API int
ntg_object_draw_set_size(ntg_object_draw* drawing, struct ntg_xy size);

/* ------------------------------------------------------ */
/* PLACEMENT */
/* ------------------------------------------------------ */


NTG_API int
ntg_object_draw_place(
        const ntg_object_draw* src_drawing,
        ntg_object_draw* dest_drawing,
        struct ntg_xy dest_start_pos);

/* ------------------------------------------------------ */


NTG_API int
ntg_object_draw_place_(
        const ntg_object_draw* src_drawing,
        ntg_stage_draw* dest_drawing,
        struct ntg_xy dest_start_pos);

/* ------------------------------------------------------ */
/* CELLS */
/* ------------------------------------------------------ */

static inline struct ntg_vcell
ntg_object_draw_get(const ntg_object_draw* drawing, struct ntg_xy pos)
{
    if(!drawing) return ntg_vcell_new_default();

    return ntg_vcell_vecgrid_get(&drawing->priv.data, pos);
}


static inline void
ntg_object_draw_set(ntg_object_draw* drawing, struct ntg_vcell cell, struct ntg_xy pos)
{
    if(!drawing) return;
    ntg_vcell_vecgrid_set(&drawing->priv.data, cell, pos);
}

#endif // NTG_OBJECT_DRAW_H
