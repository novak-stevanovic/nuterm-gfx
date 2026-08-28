#ifndef NTG_WIDGET_DRAW_H
#define NTG_WIDGET_DRAW_H

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

struct ntg_widget_draw
{
    struct
    {
        struct ntg_vcell_vec vcell_vec;
    } priv;
    struct
    {
        ntg_xy size;
    } ro;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_widget_draw_init(ntg_widget_draw* drawing);

NTG_API int
ntg_widget_draw_deinit(ntg_widget_draw* drawing);

NTG_API int
ntg_widget_draw_set_size(ntg_widget_draw* drawing, ntg_xy size);

NTG_API int
ntg_widget_draw_place(
        const ntg_widget_draw* src_drawing,
        ntg_widget_draw* dest_drawing,
        ntg_xy dest_start_pos);

NTG_API int
ntg_widget_draw_place_(
        const ntg_widget_draw* src_drawing,
        ntg_stage_draw* dest_drawing,
        ntg_xy dest_start_pos);

/* ------------------------------------------------------ */
/* CELLS */
/* ------------------------------------------------------ */

static inline struct ntg_vcell
ntg_widget_draw_get(const ntg_widget_draw* drawing, ntg_xy pos)
{
    if(!drawing) return NTG_VCELL_ZERO;

    ntg_xy size = drawing->ro.size;

    if((pos.x < size.x) && (pos.y < size.y))
        return drawing->priv.vcell_vec.data[pos.y * size.x + pos.x];
    else
        return NTG_VCELL_ZERO;
}

static inline void
ntg_widget_draw_set(ntg_widget_draw* drawing, struct ntg_vcell cell, ntg_xy pos)
{
    if(!drawing) return;

    ntg_xy size = drawing->ro.size;

    if((pos.x < size.x) && (pos.y < size.y))
        drawing->priv.vcell_vec.data[pos.y * size.x + pos.x] = cell;
}

#endif // NTG_WIDGET_DRAW_H
