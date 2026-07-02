#ifndef NTG_STAGE_DRAWING_H
#define NTG_STAGE_DRAWING_H

#include "shared/ntg_shared.h"
#include "base/ntg_cell.h"
#include "base/ntg_xy.h"

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

struct ntg_stage_drawing
{
    ntg_cell_vecgrid __data;
};

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* Initializes an empty rendered-cell stage drawing. */

/* ERROR CODES: */
/* - `NTG_ERR_INVALID_ARG`: `drawing` is `NULL`. */
void ntg_stage_drawing_init(ntg_stage_drawing* drawing, int* out_status);
/* Releases stage-drawing storage. Passing `NULL` has no effect. */
void ntg_stage_drawing_deinit(ntg_stage_drawing* drawing);

/* Gets stage-drawing dimensions. */

/* RETURN VALUE: The drawing size, or `(0, 0)` when `drawing` is `NULL`. */
struct ntg_xy ntg_stage_drawing_get_size(const ntg_stage_drawing* drawing);

/* Resizes a stage drawing, preserves overlapping rendered cells, and fills newly added cells */
/* with the default cell. `size_cap` caps only extra reserved capacity. */

/* ERROR CODES: */
/* - `NTG_ERR_INVALID_ARG`: `drawing` is `NULL`. */
/* - `NTG_ERR_ALLOC_FAIL`: drawing storage cannot be allocated. */
/* - `NTG_ERR_OUT_OF_BOUNDS`: a dimension product or byte allocation size would overflow */
/*   `size_t`. */
void ntg_stage_drawing_set_size(
        ntg_stage_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap,
        int* out_status);

/* Reads a rendered cell from a stage drawing. */

/* RETURN VALUE: The stored cell, or the default cell when the drawing is `NULL` or the position */
/*               is out of bounds. */
static inline struct ntg_cell
ntg_stage_drawing_get(const ntg_stage_drawing* drawing, struct ntg_xy pos)
{
    if(!drawing) return ntg_cell_default();

    return ntg_cell_vecgrid_get(&drawing->__data, pos);
}

/* Writes a rendered cell in a stage drawing. A `NULL` drawing or out-of-bounds position is */
/* ignored. */
static inline void
ntg_stage_drawing_set(ntg_stage_drawing* drawing, struct ntg_cell cell, struct ntg_xy pos)
{
    if(!drawing) return;

    ntg_cell_vecgrid_set(&drawing->__data, cell, pos);
}

#endif // NTG_STAGE_DRAWING_H
