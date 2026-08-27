#ifndef NTG_OBJECT_DRAW_H
#define NTG_OBJECT_DRAW_H

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

GENC_VECTOR_INLINE(ntg_vcell_vec, struct ntg_vcell, 1.5)

struct ntg_object_draw
{
    struct
    {
        struct ntg_vcell_vec vcell_vec;
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
ntg_object_draw_init(ntg_object_draw* drawing);

NTG_API int
ntg_object_draw_deinit(ntg_object_draw* drawing);

NTG_API int
ntg_object_draw_set_size(ntg_object_draw* drawing, struct ntg_xy size);

NTG_API int
ntg_object_draw_place(
        const ntg_object_draw* src_drawing,
        ntg_object_draw* dest_drawing,
        struct ntg_xy dest_start_pos);

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
    if(!drawing) return NTG_VCELL_ZERO;

    struct ntg_xy size = drawing->ro.size;

    if((pos.x < size.x) && (pos.y < size.y))
        return drawing->priv.vcell_vec.data[pos.y * size.x + pos.x];
    else
        return NTG_VCELL_ZERO;
}

static inline void
ntg_object_draw_set(ntg_object_draw* drawing, struct ntg_vcell cell, struct ntg_xy pos)
{
    if(!drawing) return;

    struct ntg_xy size = drawing->ro.size;

    if((pos.x < size.x) && (pos.y < size.y))
        drawing->priv.vcell_vec.data[pos.y * size.x + pos.x] = cell;
}

#endif // NTG_OBJECT_DRAW_H
