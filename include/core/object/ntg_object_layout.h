#ifndef NTG_OBJECT_LAYOUT_H
#define NTG_OBJECT_LAYOUT_H

#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"
#include "base/ntg_cell.h"
#include <stdint.h>

#define NTG_OBJECT_DIRTY_NONE 0
#define NTG_OBJECT_DIRTY_HMEASURE (1 << 0)
#define NTG_OBJECT_DIRTY_HCONSTRAIN (1 << 1)
#define NTG_OBJECT_DIRTY_VMEASURE (1 << 2)
#define NTG_OBJECT_DIRTY_VCONSTRAIN (1 << 3)
#define NTG_OBJECT_DIRTY_ARRANGE (1 << 4)
#define NTG_OBJECT_DIRTY_DRAW (1 << 5)
#define NTG_OBJECT_DIRTY_RENDER (1 << 6)

#define NTG_OBJECT_DIRTY_MEASURE ( \
    NTG_OBJECT_DIRTY_HMEASURE | \
    NTG_OBJECT_DIRTY_VMEASURE)

#define NTG_OBJECT_DIRTY_CONSTRAIN ( \
    NTG_OBJECT_DIRTY_HCONSTRAIN | \
    NTG_OBJECT_DIRTY_VCONSTRAIN)

#define NTG_OBJECT_DIRTY_FULL ( \
    NTG_OBJECT_DIRTY_HMEASURE | \
    NTG_OBJECT_DIRTY_HCONSTRAIN | \
    NTG_OBJECT_DIRTY_VMEASURE | \
    NTG_OBJECT_DIRTY_VCONSTRAIN | \
    NTG_OBJECT_DIRTY_ARRANGE | \
    NTG_OBJECT_DIRTY_DRAW | \
    NTG_OBJECT_DIRTY_RENDER)

/* ========================================================================== */
/* PUBLIC - TYPES */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* MEASURE PHASE */
/* ------------------------------------------------------ */

struct ntg_object_measure
{
    size_t min_size, nat_size, max_size, grow;
};

/* Compares minimum, natural, maximum, and growth values of two one-dimensional
 * object measurements. */
static inline bool ntg_object_measure_are_equal(
        struct ntg_object_measure m1,
        struct ntg_object_measure m2)
{
    return ((m1.min_size == m2.min_size) &&
            (m1.nat_size == m2.nat_size) &&
            (m1.max_size == m2.max_size) &&
            (m1.grow == m2.grow));
}

/* ------------------------------------------------------ */
/* CONSTRAIN PHASE */
/* ------------------------------------------------------ */

/* Looks up the constrained size associated with `object` in a temporary size
 * map.
 *
 * RETURN VALUE:
 * The stored size, or `NTG_SIZE_MAX` when the map/object is invalid or the
 * object is absent. */
NTG_API size_t
ntg_object_size_map_get(
        const ntg_object_size_map* map,
        const ntg_object* object);

/* ------------------------------------------------------ */

/* Stores a constrained size for an object already represented in the temporary
 * size map. Invalid or absent entries are ignored. */
NTG_API void
ntg_object_size_map_set(
        ntg_object_size_map* map,
        const ntg_object* object,
        size_t size);

/* ------------------------------------------------------ */
/* ARRANGE PHASE */
/* ------------------------------------------------------ */

/* Looks up the arranged position associated with `object` in a temporary
 * position map.
 *
 * RETURN VALUE:
 * The stored position, or `NTG_XY_MAX` when the map/object is invalid or the
 * object is absent. */
NTG_API struct ntg_xy
ntg_object_pos_map_get(
        const ntg_object_pos_map* map,
        const ntg_object* object);

/* ------------------------------------------------------ */

/* Stores an arranged position for an object already represented in the
 * temporary position map. Invalid or absent entries are ignored. */
NTG_API void
ntg_object_pos_map_set(
        ntg_object_pos_map* map,
        const ntg_object* object,
        struct ntg_xy pos);

/* ------------------------------------------------------ */
/* DRAW PHASE */
/* ------------------------------------------------------ */

struct ntg_object_tmp_drawing
{
    struct ntg_vcell* data;
    struct ntg_xy size;
};

/* Reads a virtual cell from a temporary arena-backed drawing.
 *
 * RETURN VALUE:
 * The stored cell, or the default full-space virtual cell when the drawing or
 * position is invalid. */
static inline struct ntg_vcell
ntg_object_tmp_drawing_get(const ntg_object_tmp_drawing* drawing, struct ntg_xy pos)
{
    if(!drawing) return ntg_vcell_default();

    if(ntg_xy_is_lesser(pos, drawing->size))
        return drawing->data[drawing->size.x * pos.y + pos.x];
    else
        return ntg_vcell_default();
}

/* Writes a virtual cell into a temporary arena-backed drawing. Invalid drawings
 * or positions are ignored. */
static inline void ntg_object_tmp_drawing_set(
        ntg_object_tmp_drawing* drawing,
        struct ntg_vcell cell,
        struct ntg_xy pos)
{
    if(!drawing) return;

    if(ntg_xy_is_lesser(pos, drawing->size))
        drawing->data[drawing->size.x * pos.y + pos.x] = cell;
}

/* ========================================================================== */
/* PUBLIC - FUNCTIONS */
/* ========================================================================== */

/* ORs the selected dirty flags into this object and marks its owning scene
 * dirty. It does not copy the flags to ancestor objects. */
NTG_API void
ntg_object_mark_dirty(ntg_object* object, uint8_t dirty);

/* ------------------------------------------------------ */
/* MEASURE & SIZE HELPERS */
/* ------------------------------------------------------ */

/* Gets the object's minimum outer size.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_min_size(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's natural outer size.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_nat_size(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's maximum outer size.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_max_size(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's current arranged outer size.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_size(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets minimum, natural, maximum, and growth values for one axis of the
 * object's outer box.
 *
 * RETURN VALUE:
 * The one-dimensional measurement; all zeros for a `NULL` object. */
NTG_API struct ntg_object_measure
ntg_object_get_measure(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

/* Gets the current outer size along `orient`.
 *
 * RETURN VALUE:
 * The selected size component, or `0` for a `NULL` object. */
NTG_API size_t
ntg_object_get_size_1d(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

/* Gets the object's minimum content-box size.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_min_size_cont(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's natural content-box size.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_nat_size_cont(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's maximum content-box size.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_max_size_cont(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's current content-box size.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_size_cont(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets minimum, natural, maximum, and growth values for one axis of the
 * object's content box.
 *
 * RETURN VALUE:
 * The one-dimensional measurement; all zeros for a `NULL` object. */
NTG_API struct ntg_object_measure
ntg_object_get_measure_cont(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

/* Gets the current content-box size along `orient`.
 *
 * RETURN VALUE:
 * The selected size component, or `0` for a `NULL` object. */
NTG_API size_t
ntg_object_get_size_1d_cont(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

/* Gets the content-width constraint used during vertical measurement;
 * horizontal measurement has no for-size constraint.
 *
 * RETURN VALUE:
 * The content width for vertical orientation, `NTG_SIZE_MAX` for horizontal
 * orientation, or `0` for a `NULL` object. */
NTG_API size_t
ntg_object_get_for_size_cont(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

/* Gets the object's minimum padding-box size, excluding the border.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_min_size_pad(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's natural padding-box size, excluding the border.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_nat_size_pad(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's maximum padding-box size, excluding the border.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_max_size_pad(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets the object's current padding-box size, excluding the border.
 *
 * RETURN VALUE:
 * The requested two-dimensional size; `(0, 0)` for `NULL`. */
NTG_API struct ntg_xy
ntg_object_get_size_pad(const ntg_object* object);

/* ------------------------------------------------------ */

/* Gets minimum, natural, maximum, and growth values for one axis of the
 * object's padding box.
 *
 * RETURN VALUE:
 * The one-dimensional measurement; all zeros for a `NULL` object. */
NTG_API struct ntg_object_measure
ntg_object_get_measure_pad(const ntg_object* object, ntg_orient orient);

/* ------------------------------------------------------ */

/* Gets the current padding-box size along `orient`.
 *
 * RETURN VALUE:
 * The selected size component, or `0` for a `NULL` object. */
NTG_API size_t
ntg_object_get_size_1d_pad(const ntg_object* object, ntg_orient orient);

/* ========================================================================== */
/* PROTECTED */
/* ========================================================================== */

/* ========================================================================== */
/* INTERNAL */
/* ========================================================================== */

/* Measures one object horizontally, applies decorator and user constraints,
 * stores changed measurements, and invalidates dependent phases and its parent
 * when needed. */
void _ntg_object_hmeasure(ntg_object* object, sarena* arena);
/* Computes one object’s horizontal border and padding sizes, then assigns
 * widths to its direct children through the constrain callback. */
void _ntg_object_hconstrain(ntg_object* object, sarena* arena);
/* Measures one object vertically using its resolved width, applies decorator
 * and user constraints, and stores changed measurements. */
void _ntg_object_vmeasure(ntg_object* object, sarena* arena);
/* Computes one object’s vertical border and padding sizes, then assigns heights
 * to its direct children through the constrain callback. */
void _ntg_object_vconstrain(ntg_object* object, sarena* arena);
/* Runs one object’s decorator and optional custom fixup state, clears its
 * repeat flag, and marks horizontal constraint dirty when another pass is
 * requested.
 *
 * RETURN VALUE:
 * `true` when this object requests another layout pass; otherwise `false`. */
bool _ntg_object_fixup(ntg_object* object, sarena* arena);
/* Uses one object’s arrange callback to assign positions to its direct
 * children, offsetting for decorators and clamping positions to the parent
 * bounds. */
void _ntg_object_arrange(ntg_object* object, sarena* arena);
/* Resizes and rebuilds one object’s drawing, then marks it for rendering. A
 * `NULL` object or arena is ignored. */
void _ntg_object_draw(ntg_object* object, sarena* arena);

/* Sets the root width and invalidates dependent vertical layout when it
 * changes. */
void _ntg_object_root_set_hsize(ntg_object* object, size_t size);
/* Sets the root height and invalidates arrangement/drawing when it changes. */
void _ntg_object_root_set_vsize(ntg_object* object, size_t size);
/* Assigns the position of a root object. It does not mark any phase dirty;
 * non-root or `NULL` objects are ignored. */
void _ntg_object_root_set_pos(ntg_object* object, struct ntg_xy pos);

/* Clears the selected dirty bits on one object. It does not recurse into
 * descendants. */
void _ntg_object_clean(ntg_object* object, uint8_t clean);

#endif // NTG_OBJECT_LAYOUT_H
