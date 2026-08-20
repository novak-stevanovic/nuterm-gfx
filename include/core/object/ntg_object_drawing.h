#ifndef NTG_OBJECT_DRAWING_H
#define NTG_OBJECT_DRAWING_H

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

struct ntg_object_drawing
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
ntg_object_drawing_init(ntg_object_drawing* drawing);

/* ------------------------------------------------------ */

NTG_API int
ntg_object_drawing_deinit(ntg_object_drawing* drawing);

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

NTG_API struct ntg_xy
ntg_object_drawing_get_size(const ntg_object_drawing* drawing);

/* ------------------------------------------------------ */

NTG_API int
ntg_object_drawing_set_size(
        ntg_object_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap);

/* ------------------------------------------------------ */
/* PLACEMENT */
/* ------------------------------------------------------ */


NTG_API int
ntg_object_drawing_place(
        const ntg_object_drawing* src_drawing,
        ntg_object_drawing* dest_drawing,
        struct ntg_xy dest_start_pos);

/* ------------------------------------------------------ */


NTG_API int
ntg_object_drawing_place_(
        const ntg_object_drawing* src_drawing,
        ntg_stage_drawing* dest_drawing,
        struct ntg_xy dest_start_pos);

/* ------------------------------------------------------ */
/* CELLS */
/* ------------------------------------------------------ */


static inline struct ntg_vcell
ntg_object_drawing_get(const ntg_object_drawing* drawing, struct ntg_xy pos)
{
    if(!drawing)
        return ntg_vcell_new_default();

    return ntg_vcell_vecgrid_get(&drawing->priv.data, pos);
}


static inline int
ntg_object_drawing_set(ntg_object_drawing* drawing, struct ntg_vcell cell, struct ntg_xy pos)
{
    if(!drawing) return NTG_ERR_INV_ARG;

    (void)ntg_vcell_vecgrid_set(&drawing->priv.data, cell, pos);

    return 0;
}

#endif // NTG_OBJECT_DRAWING_H
