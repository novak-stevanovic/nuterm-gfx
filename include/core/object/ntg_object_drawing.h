#ifndef NTG_OBJECT_DRAWING_H
#define NTG_OBJECT_DRAWING_H

#include "shared/ntg_shared.h"
#include "base/ntg_cell.h"

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_object_drawing
{
    ntg_vcell_vecgrid __data;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes an empty virtual-cell drawing. */
NTG_API void
ntg_object_drawing_init(ntg_object_drawing* drawing);

/* ------------------------------------------------------ */

/* Releases drawing storage. Passing `NULL` has no effect. */
NTG_API void
ntg_object_drawing_deinit(ntg_object_drawing* drawing);

/* ------------------------------------------------------ */
/* SIZE */
/* ------------------------------------------------------ */

/* Gets drawing dimensions.
 *
 * RETURN VALUE:
 * The drawing size, or `(0, 0)` when `drawing` is `NULL`. */
NTG_API struct ntg_xy
ntg_object_drawing_get_size(const ntg_object_drawing* drawing);

/* ------------------------------------------------------ */

/* Resizes a drawing, preserves overlapping virtual cells, and fills newly added
 * cells with the default full-space virtual cell. `size_cap` caps only extra
 * reserved capacity.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `drawing` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: drawing storage cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: the underlying virtual-cell resize fails for another
 *   reason, including arithmetic overflow. */
NTG_API void
ntg_object_drawing_set_size(
        ntg_object_drawing* drawing,
        struct ntg_xy size,
        struct ntg_xy size_cap,
        int* out_status);

/* ------------------------------------------------------ */
/* PLACEMENT */
/* ------------------------------------------------------ */

/* Copies every virtual cell from `src_drawing` directly into `dest_drawing` at
 * `dest_start_pos`. Source and destination must be different drawings.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: either drawing is `NULL`, or both pointers designate
 *   the same drawing.
 * - `NTG_ERR_OUT_OF_BOUNDS`: the nonempty source start or end is greater than
 *   or equal to the destination size. The current check rejects a source ending
 *   exactly at the destination boundary. */
NTG_API void
ntg_object_drawing_place(
        const ntg_object_drawing* src_drawing,
        ntg_object_drawing* dest_drawing,
        struct ntg_xy dest_start_pos,
        int* out_status);

/* ------------------------------------------------------ */

/* Composites every source virtual cell over a rendered stage cell at
 * `dest_start_pos` using `ntg_vcell_overwrite`.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: either drawing is `NULL`.
 * - `NTG_ERR_OUT_OF_BOUNDS`: the nonempty source start or end is greater than
 *   or equal to the destination size. The current check rejects a source ending
 *   exactly at the destination boundary. */
NTG_API void
ntg_object_drawing_place_(
        const ntg_object_drawing* src_drawing,
        ntg_stage_drawing* dest_drawing,
        struct ntg_xy dest_start_pos,
        int* out_status);

/* ------------------------------------------------------ */
/* CELLS */
/* ------------------------------------------------------ */

/* Reads a virtual cell from a drawing.
 *
 * RETURN VALUE:
 * The stored cell, or the default full-space virtual cell when the drawing is
 * `NULL` or the position is out of bounds. */
static inline struct ntg_vcell
ntg_object_drawing_get(const ntg_object_drawing* drawing, struct ntg_xy pos)
{
    if(!drawing)
        return ntg_vcell_def();

    return ntg_vcell_vecgrid_get(&drawing->__data, pos);
}

/* Writes a virtual cell in a drawing. A `NULL` drawing or an out-of-bounds
 * position is ignored. */
static inline void
ntg_object_drawing_set(ntg_object_drawing* drawing, struct ntg_vcell cell, struct ntg_xy pos)
{
    if(!drawing) return;

    ntg_vcell_vecgrid_set(&drawing->__data, cell, pos);
}

#endif // NTG_OBJECT_DRAWING_H
