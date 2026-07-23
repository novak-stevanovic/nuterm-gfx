#ifndef NTG_XY_H
#define NTG_XY_H

#include "shared/ntg_shared.h"

#define NTG_SIZE_MAX 2000

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

struct ntg_xy
{
    size_t x, y;
};

struct ntg_dxy
{
    ssize_t x, y;
};

enum ntg_orient
{
    NTG_ORIENT_H,
    NTG_ORIENT_V
};

enum ntg_align
{
    /* Align left on the x axis, or top on the y axis. */
    NTG_ALIGN_1,

    /* Align to the center of the selected axis. */
    NTG_ALIGN_2,

    /* Align right on the x axis, or bottom on the y axis. */
    NTG_ALIGN_3
};


struct ntg_oxy
{
    size_t prim_val, sec_val;
    ntg_orient orient;
};

static const struct ntg_xy NTG_XY_MAX = { NTG_SIZE_MAX, NTG_SIZE_MAX };
static const struct ntg_xy NTG_XY_MIN = { 0, 0 };
static const struct ntg_xy NTG_XY_UNSET = { 0, 0 };

static const struct ntg_dxy NTG_DXY_MAX = { NTG_SIZE_MAX, NTG_SIZE_MAX };
static const struct ntg_dxy NTG_DXY_MIN = { 0, 0 };
static const struct ntg_dxy NTG_DXY_UNSET = { 0, 0 };

enum ntg_side
{
    NTG_SIDE_N,
    NTG_SIDE_E,
    NTG_SIDE_S,
    NTG_SIDE_W
};

struct ntg_insets
{
    size_t n, e, s, w;
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* NTG_XY */
/* ------------------------------------------------------ */

/* Creates an unsigned two-dimensional coordinate or size.
 *
 * RETURN VALUE:
 * A coordinate with the supplied `x` and `y` values. */
static inline struct ntg_xy
ntg_xy(size_t x, size_t y)
{
    return (struct ntg_xy) { .x = x, .y = y };
}

/* Normalizes a size in place: if either dimension is zero, both dimensions
 * become zero. A `NULL` pointer is ignored. */
static inline void
ntg_xy_size_(struct ntg_xy* xy)
{
    if(xy == NULL) return;

    if((xy->x == 0) || (xy->y == 0))
    {
        xy->x = 0;
        xy->y = 0;
    }
}

/* Returns the normalized form of a size: if either dimension is zero, both
 * dimensions become zero.
 *
 * RETURN VALUE:
 * The normalized size. */
static inline struct 
ntg_xy ntg_xy_size(struct ntg_xy xy)
{
    ntg_xy_size_(&xy);

    return xy;
}

/* Adds two unsigned coordinates component by component.
 *
 * RETURN VALUE:
 * The component-wise sum. */
static inline struct 
ntg_xy ntg_xy_add(struct ntg_xy a, struct ntg_xy b)
{
    return (struct ntg_xy) { .x = a.x + b.x, .y = a.y + b.y };
}

/* Subtracts two unsigned coordinates component by component, saturating each
 * result at zero.
 *
 * RETURN VALUE:
 * The saturated component-wise difference. */
static inline struct ntg_xy
ntg_xy_sub(struct ntg_xy a, struct ntg_xy b)
{
    return (struct ntg_xy) { 
        .x = (a.x > b.x) ? (a.x - b.x) : 0,
        .y = (a.y > b.y) ? (a.y - b.y) : 0,
    };
}

/* Converts a signed coordinate to an unsigned coordinate by C integer
 * conversion.
 *
 * RETURN VALUE:
 * The converted coordinate; negative components wrap according to unsigned
 * conversion rules. */
static inline struct ntg_xy
ntg_xy_from_dxy(struct ntg_dxy xy)
{
    return (struct ntg_xy) { .x = (size_t)xy.x, .y = (size_t)xy.y };
}

/* Checks whether both components of `a` are strictly greater than the
 * corresponding components of `b`. */
static inline bool
ntg_xy_is_greater(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x > b.x) && (a.y > b.y));
}

/* Checks whether both components of `a` are strictly less than the
 * corresponding components of `b`. */
static inline bool
ntg_xy_is_lesser(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x < b.x) && (a.y < b.y));
}

/* Checks whether both components of `a` are less than or equal to the
 * corresponding components of `b`. */
static inline bool
ntg_xy_is_lesser_eq(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x <= b.x) && (a.y <= b.y));
}

/* Checks whether both components of `a` are greater than or equal to the
 * corresponding components of `b`. */
static inline bool
ntg_xy_is_greater_eq(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x >= b.x) && (a.y >= b.y));
}

/* Compares two unsigned coordinates component by component. */
static inline bool
ntg_xy_are_eql(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x == b.x) && (a.y == b.y));
}

/* Clamps each component of `val` independently to the inclusive range from
 * `min` to `max`.
 *
 * RETURN VALUE:
 * The clamped coordinate. */
static inline struct
ntg_xy ntg_xy_clamp(struct ntg_xy min, struct ntg_xy val, struct ntg_xy max)
{
    if(val.x < min.x) val.x = min.x;
    else if(val.x > max.x) val.x = max.x;

    if(val.y < min.y) val.y = min.y;
    else if(val.y > max.y) val.y = max.y;

    return val;
}

/* Checks whether both dimensions are zero. */
static inline bool
ntg_xy_is_zero(struct ntg_xy size)
{
    return ((size.x == 0) && (size.y == 0));
}

/* Checks whether a size has at least one zero dimension. */
static inline bool
ntg_xy_size_is_zero(struct ntg_xy size)
{
    return ((size.x == 0) || (size.y == 0));
}

/* Swaps the horizontal and vertical components of a coordinate.
 *
 * RETURN VALUE:
 * A coordinate whose `x` is the original `y` and vice versa. */
static inline struct ntg_xy
ntg_xy_transpose(struct ntg_xy xy)
{
    return (struct ntg_xy) {
        .x = xy.y,
        .y = xy.x
    };
}

/* Selects the component that belongs to `orient`.
 *
 * RETURN VALUE:
 * `x` for horizontal orientation; `y` for any other orientation. */
static inline size_t
ntg_xy_get(struct ntg_xy xy, ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? xy.x : xy.y;
}

/* Returns `xy` with the component selected by `orient` replaced by `val`; the
 * other component is unchanged.
 *
 * RETURN VALUE:
 * The updated coordinate. */
static inline struct ntg_xy
ntg_xy_set(struct ntg_xy xy, size_t val, ntg_orient orient)
{
    if(orient == NTG_ORIENT_H)
        xy.x = val;
    else
        xy.y = val;

    return xy;
}

/* Checks whether `p` lies inside the half-open rectangle from `(0, 0)` to
 * `size`.
 *
 * RETURN VALUE:
 * `true` when `p.x < size.x` and `p.y < size.y`. */
static inline bool
ntg_xy_in_size(struct ntg_xy p, struct ntg_xy size)
{
    return (p.x < size.x) && (p.y < size.y);
}

/* Clamps an object position so an object of `size` remains inside
 * `parent_size`. If the object is at least as large as the parent on an axis,
 * that position component becomes zero.
 *
 * RETURN VALUE:
 * The adjusted position. */
static inline struct ntg_xy
ntg_xy_pos_clamp(struct ntg_xy pos, struct ntg_xy size, struct ntg_xy parent_size)
{
    if(size.x >= parent_size.x)
        pos.x = 0;
    else
    {
        size_t max_x = parent_size.x - size.x;
        if(pos.x > max_x) pos.x = max_x;
    }

    if(size.y >= parent_size.y)
        pos.y = 0;
    else
    {
        size_t max_y = parent_size.y - size.y;
        if(pos.y > max_y) pos.y = max_y;
    }

    return pos;
}

/* ------------------------------------------------------ */
/* NTG_DXY */
/* ------------------------------------------------------ */

/* Creates a signed two-dimensional coordinate.
 *
 * RETURN VALUE:
 * A coordinate with the supplied `x` and `y` values. */
static inline struct ntg_dxy
ntg_dxy(ssize_t x, ssize_t y)
{
    return (struct ntg_dxy) { .x = x, .y = y };
}

/* Converts an unsigned coordinate to a signed coordinate by C integer
 * conversion.
 *
 * RETURN VALUE:
 * The converted signed coordinate. */
static inline struct ntg_dxy
ntg_dxy_from_xy(struct ntg_xy xy)
{
    return (struct ntg_dxy) { .x = (ssize_t)xy.x, .y = (ssize_t)xy.y };
}

/* Adds two signed coordinates component by component.
 *
 * RETURN VALUE:
 * The component-wise sum. */
static inline struct ntg_dxy
ntg_dxy_add(struct ntg_dxy a, struct ntg_dxy b)
{
    return (struct ntg_dxy) { .x = a.x + b.x, .y = a.y + b.y };
}

/* Subtracts two signed coordinates component by component.
 *
 * RETURN VALUE:
 * The component-wise difference. */
static inline struct ntg_dxy
ntg_dxy_sub(struct ntg_dxy a, struct ntg_dxy b)
{
    return (struct ntg_dxy) { .x = a.x - b.x, .y = a.y - b.y };
}

/* Clamps each signed component of `val` independently to the inclusive range
 * from `min` to `max`.
 *
 * RETURN VALUE:
 * The clamped coordinate. */
static inline struct ntg_dxy
ntg_dxy_clamp(struct ntg_dxy min, struct ntg_dxy val, struct ntg_dxy max)
{
    if(val.x < min.x) val.x = min.x;
    else if(val.x > max.x) val.x = max.x;

    if(val.y < min.y) val.y = min.y;
    else if(val.y > max.y) val.y = max.y;

    return val;
}

/* Selects the signed component that belongs to `orient`.
 *
 * RETURN VALUE:
 * The selected component converted to `size_t`; negative values therefore wrap
 * according to unsigned conversion rules. */
static inline size_t
ntg_dxy_get(struct ntg_dxy xy, ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? xy.x : xy.y;
}

/* Checks whether `pos` lies in the half-open rectangle `[rec_start, rec_end)`
 * on both axes. */
static inline bool ntg_xy_is_in_rectagle(
        struct ntg_xy pos,
        struct ntg_xy rec_start,
        struct ntg_xy rec_end)
{
    return ((pos.x >= rec_start.x) && (pos.y >= rec_start.y) &&
    (pos.x < rec_end.x) && (pos.y < rec_end.y));
}

/* Converts an oriented coordinate to ordinary `x`/`y` order.
 *
 * RETURN VALUE:
 * `(p, s)` for horizontal orientation and `(s, p)` for vertical orientation. */
static inline struct ntg_xy
ntg_xy_from_oxy(struct ntg_oxy orient_xy)
{
    if(orient_xy.orient == NTG_ORIENT_H)
        return ntg_xy(orient_xy.prim_val, orient_xy.sec_val);
    else
        return ntg_xy(orient_xy.sec_val, orient_xy.prim_val);
}

/* Checks whether `pos` lies in the signed half-open rectangle `[rec_start,
 * rec_end)` on both axes. */
static inline bool ntg_dxy_is_in_rectagle(
        struct ntg_dxy pos,
        struct ntg_dxy rec_start,
        struct ntg_dxy rec_end)
{
    return ((pos.x >= rec_start.x) && (pos.y >= rec_start.y) &&
    (pos.x < rec_end.x) && (pos.y < rec_end.y));
}

/* ------------------------------------------------------ */
/* NTG_ORIENT */
/* ------------------------------------------------------ */

/* Returns the orientation perpendicular to `orient`.
 *
 * RETURN VALUE:
 * Vertical for horizontal input; horizontal for any other input. */
static inline ntg_orient
ntg_orient_get_other(ntg_orient ort)
{
    return (ort == NTG_ORIENT_H) ? NTG_ORIENT_V : NTG_ORIENT_H;
}

/* ------------------------------------------------------ */
/* NTG_OXY */
/* ------------------------------------------------------ */

/* Creates an oriented coordinate with primary and secondary components.
 *
 * RETURN VALUE:
 * The constructed oriented coordinate. */
static inline struct ntg_oxy
ntg_oxy(size_t prim_val, size_t sec_val, ntg_orient orient)
{
    return (struct ntg_oxy) {
        .prim_val = prim_val,
        .sec_val = sec_val,
        .orient = orient
    };
}

/* Normalizes an oriented size: if either component is zero, both components
 * become zero.
 *
 * RETURN VALUE:
 * The normalized oriented size. */
static inline struct ntg_oxy
ntg_oxy_size(struct ntg_oxy oxy)
{
    if((oxy.prim_val == 0) || (oxy.sec_val == 0))
        return ntg_oxy(0, 0, oxy.orient);
    else
        return oxy;
}

/* Converts an ordinary coordinate to primary/secondary order for `orient`.
 *
 * RETURN VALUE:
 * `(x, y)` for horizontal orientation and `(y, x)` for vertical orientation. */
static inline struct ntg_oxy
ntg_oxy_from_xy(struct ntg_xy xy, ntg_orient orient)
{
    if(orient == NTG_ORIENT_H)
        return ntg_oxy(xy.x, xy.y, NTG_ORIENT_H);
    else
        return ntg_oxy(xy.y, xy.x, NTG_ORIENT_V);
}

/* ------------------------------------------------------ */
/* NTG_INSETS */
/* ------------------------------------------------------ */

/* Creates top, right, bottom, and left inset values.
 *
 * RETURN VALUE:
 * The constructed inset set. */
static inline struct ntg_insets ntg_insets(size_t n, size_t e, size_t s, size_t w)
{
    return (struct ntg_insets) {
        .n = n,
        .e = e,
        .s = s,
        .w = w
    };
}

/* Adds the left and right insets.
 *
 * RETURN VALUE:
 * The total horizontal inset. */
static inline size_t ntg_insets_hsum(struct ntg_insets insets)
{
    return insets.e + insets.w;
}

/* Adds the top and bottom insets.
 *
 * RETURN VALUE:
 * The total vertical inset. */
static inline size_t ntg_insets_vsum(struct ntg_insets insets)
{
    return insets.n + insets.s;
}

/* Adds the two insets along the requested orientation.
 *
 * RETURN VALUE:
 * Left plus right for horizontal orientation; top plus bottom otherwise. */
static inline size_t ntg_insets_sum(struct ntg_insets insets, ntg_orient orient)
{
    if(orient == NTG_ORIENT_H)
        return ntg_insets_hsum(insets);
    else
        return ntg_insets_vsum(insets);
}

/* Adds two inset sets side by side.
 *
 * RETURN VALUE:
 * The component-wise sum of the insets. */
static inline struct ntg_insets
ntg_insets_add(struct ntg_insets i1, struct ntg_insets i2)
{
    return (struct ntg_insets) {
        .n = i1.n + i2.n,
        .e = i1.e + i2.e,
        .s = i1.s + i2.s,
        .w = i1.w + i2.w
    };
}

/* Checks whether all four insets are zero. */
static inline bool ntg_insets_is_zero(struct ntg_insets insets)
{
    return ((insets.n == 0) && (insets.s == 0) &&
            (insets.e == 0) && (insets.w == 0));
}

/* Creates an inset set whose four values are zero. The input value is ignored.
 *
 * RETURN VALUE:
 * Zero insets. */
static inline struct ntg_insets ntg_insets_zero(struct ntg_insets insets)
{
    return (struct ntg_insets) {0};
}

/* Compares two inset sets component by component. */
static inline bool ntg_insets_are_eql(struct ntg_insets insets1, struct ntg_insets insets2)
{
    return ((insets1.n == insets2.n) &&
            (insets1.e == insets2.e) &&
            (insets1.s == insets2.s) &&
            (insets1.w == insets2.w));
}

/* ------------------------------------------------------ */
/* NTG_SIDE */
/* ------------------------------------------------------ */

/* Gets the axis on which a side lies.
 *
 * RETURN VALUE:
 * Vertical for north or south; horizontal for east or west. */
static inline ntg_orient ntg_side_get_orient(ntg_side side)
{
    if((side == NTG_SIDE_N) || (side == NTG_SIDE_S))
        return NTG_ORIENT_V;
    else
        return NTG_ORIENT_H;
}

/* ------------------------------------------------------ */
/* NTG_ALIGN */
/* ------------------------------------------------------ */

/* Computes the non-negative offset selected by `align` for placing `inner_size`
 * within `outer_size`. Values larger than `outer_size` are clamped before
 * calculation.
 *
 * RETURN VALUE:
 * Zero for start alignment, the remaining space for end alignment, or half the
 * remaining space for center alignment. */
static inline size_t
ntg_align_offset(size_t inner_size, size_t outer_size, ntg_align align)
{
    if(inner_size > outer_size) inner_size = outer_size;

    if(align == NTG_ALIGN_1)
        return 0;
    else if(align == NTG_ALIGN_2)
        return (outer_size - inner_size) / 2;
    else
        return (outer_size - inner_size);
}

/* Computes the signed offset selected by `align` for placing `inner_size`
 * within `outer_size`. Unlike `ntg_align_offset`, this function does not clamp
 * an oversized inner value.
 *
 * RETURN VALUE:
 * Zero for start alignment, `outer_size - inner_size` for end alignment, or
 * half that difference for center alignment. */
static inline ssize_t
ntg_align_offset_d(ssize_t inner_size, ssize_t outer_size, ntg_align align)
{
    if(align == NTG_ALIGN_1)
        return 0;
    else if(align == NTG_ALIGN_2)
        return (outer_size - inner_size) / 2;
    else
        return (outer_size - inner_size);
}

#endif // NTG_XY_H
