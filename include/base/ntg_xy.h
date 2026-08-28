#ifndef NTG_XY_H
#define NTG_XY_H

#include "shared/ntg_shared.h"
#include "shared/ntg_error.h"

#define NTG_SIZE_MAX 10000

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

struct ntg_xy
{
    size_t x, y;
};

GENC_OPT_INLINE_DEF(ntg_xy_opt, ntg_xy)

struct ntg_dxy
{
    ssize_t x, y;
};

enum ntg_orient
{
    NTG_ORIENT_H = 0,
    NTG_ORIENT_V
};

enum ntg_align
{
    NTG_ALIGN_1 = 0,

    NTG_ALIGN_2,
    
    NTG_ALIGN_3
};

static const ntg_xy NTG_XY_MAX = { NTG_SIZE_MAX, NTG_SIZE_MAX };
static const ntg_xy NTG_XY_MIN = { 0, 0 };
static const ntg_xy NTG_XY_ZERO = {0};

static const ntg_dxy NTG_DXY_MAX = { NTG_SIZE_MAX, NTG_SIZE_MAX };
static const ntg_dxy NTG_DXY_MIN = { -NTG_SIZE_MAX, -NTG_SIZE_MAX };
static const ntg_dxy NTG_DXY_ZERO = {0};

enum ntg_side
{
    NTG_SIDE_N = 0,
    NTG_SIDE_E,
    NTG_SIDE_S,
    NTG_SIDE_W
};

struct ntg_insets
{
    size_t n, e, s, w;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* NTG_XY */
/* ------------------------------------------------------ */

static inline ntg_xy
ntg_xy_new(size_t x, size_t y)
{
    return (ntg_xy) { .x = x, .y = y };
}

static inline ntg_xy
ntg_xy_new_orient(size_t prim, size_t sec, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? ntg_xy_new(prim, sec) : ntg_xy_new(sec, prim);
}

static inline ntg_xy
ntg_xy_size(ntg_xy xy)
{
    if(!xy.x || !xy.y)
        return (ntg_xy) {0};
    else
        return xy;
}

static inline ntg_xy
ntg_xy_add(ntg_xy a, ntg_xy b)
{
    return (ntg_xy) { .x = a.x + b.x, .y = a.y + b.y };
}

static inline ntg_xy
ntg_xy_sub(ntg_xy a, ntg_xy b)
{
    return (ntg_xy) { 
        .x = (a.x > b.x) ? (a.x - b.x) : 0,
        .y = (a.y > b.y) ? (a.y - b.y) : 0,
    };
}

static inline ntg_xy
ntg_xy_from_dxy(ntg_dxy xy)
{
    return (ntg_xy) {
        .x = ((xy.x >= 0) ? xy.x : 0),
        .y = ((xy.y >= 0) ? xy.y : 0),
    };
}

static inline bool
ntg_xy_is_gt(ntg_xy a, ntg_xy b)
{
    return ((a.x > b.x) && (a.y > b.y));
}

static inline bool
ntg_xy_is_lt(ntg_xy a, ntg_xy b)
{
    return ((a.x < b.x) && (a.y < b.y));
}

static inline bool
ntg_xy_is_le(ntg_xy a, ntg_xy b)
{
    return ((a.x <= b.x) && (a.y <= b.y));
}

static inline bool
ntg_xy_is_ge(ntg_xy a, ntg_xy b)
{
    return ((a.x >= b.x) && (a.y >= b.y));
}

static inline bool
ntg_xy_are_eql(ntg_xy a, ntg_xy b)
{
    return ((a.x == b.x) && (a.y == b.y));
}

static inline ntg_xy
ntg_xy_clamp(ntg_xy min, ntg_xy val, ntg_xy max)
{
    if(val.x < min.x) val.x = min.x;
    else if(val.x > max.x) val.x = max.x;

    if(val.y < min.y) val.y = min.y;
    else if(val.y > max.y) val.y = max.y;

    return val;
}

static inline bool
ntg_xy_is_in_rect(ntg_xy pos, ntg_xy start, ntg_xy end)
{
    return ((pos.x >= start.x) && (pos.y >= start.y) &&
            (pos.x < end.x) && (pos.y < end.y));
}

static inline bool
ntg_xy_is_zero(ntg_xy size)
{
    return ((size.x == 0) && (size.y == 0));
}

static inline bool
ntg_xy_is_zero_any(ntg_xy size)
{
    return ((size.x == 0) || (size.y == 0));
}

static inline ntg_xy
ntg_xy_transpose(ntg_xy xy)
{
    return (ntg_xy) {
        .x = xy.y,
        .y = xy.x
    };
}

static inline size_t
ntg_xy_get(ntg_xy xy, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? xy.x : xy.y;
}

static inline size_t
ntg_xy_get_other(ntg_xy xy, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_V) ? xy.x : xy.y;
}

static inline ntg_xy
ntg_xy_set(ntg_xy xy, size_t val, enum ntg_orient orient)
{
    if(orient == NTG_ORIENT_H)
        xy.x = val;
    else
        xy.y = val;

    return xy;
}

static inline ntg_xy
ntg_xy_pos_clamp(ntg_xy pos, ntg_xy size, ntg_xy parent_size)
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

static inline ntg_dxy
ntg_dxy_new(ssize_t x, ssize_t y)
{
    return (ntg_dxy) { .x = x, .y = y };
}

static inline ntg_dxy
ntg_dxy_from_xy(ntg_xy xy)
{
    return (ntg_dxy) { .x = (ssize_t)xy.x, .y = (ssize_t)xy.y };
}

static inline ntg_dxy
ntg_dxy_add(ntg_dxy a, ntg_dxy b)
{
    return (ntg_dxy) { .x = a.x + b.x, .y = a.y + b.y };
}

static inline ntg_dxy
ntg_dxy_sub(ntg_dxy a, ntg_dxy b)
{
    return (ntg_dxy) { .x = a.x - b.x, .y = a.y - b.y };
}

static inline ntg_dxy
ntg_dxy_clamp(ntg_dxy min, ntg_dxy val, ntg_dxy max)
{
    if(val.x < min.x) val.x = min.x;
    else if(val.x > max.x) val.x = max.x;

    if(val.y < min.y) val.y = min.y;
    else if(val.y > max.y) val.y = max.y;

    return val;
}

static inline size_t
ntg_dxy_get(ntg_dxy xy, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? xy.x : xy.y;
}

static inline bool
ntg_dxy_is_in_rect(ntg_dxy pos, ntg_dxy start, ntg_dxy end)
{
    return ((pos.x >= start.x) && (pos.y >= start.y) &&
            (pos.x < end.x) && (pos.y < end.y));
}

/* ------------------------------------------------------ */
/* NTG_ORIENT */
/* ------------------------------------------------------ */

static inline enum ntg_orient
ntg_orient_other(enum ntg_orient ort)
{
    return (ort == NTG_ORIENT_H) ? NTG_ORIENT_V : NTG_ORIENT_H;
}

/* ------------------------------------------------------ */
/* NTG_INSETS */
/* ------------------------------------------------------ */

static inline ntg_insets
ntg_insets_new(size_t n, size_t e, size_t s, size_t w)
{
    return (ntg_insets) { .n = n, .e = e, .s = s, .w = w };
}

static inline size_t
ntg_insets_hsum(ntg_insets insets)
{
    return insets.e + insets.w;
}

static inline size_t
ntg_insets_vsum(ntg_insets insets)
{
    return insets.n + insets.s;
}

static inline size_t
ntg_insets_sum(ntg_insets insets, enum ntg_orient orient)
{
    if(orient == NTG_ORIENT_H)
        return ntg_insets_hsum(insets);
    else
        return ntg_insets_vsum(insets);
}

static inline ntg_insets
ntg_insets_add(ntg_insets i1, ntg_insets i2)
{
    return (ntg_insets) {
        .n = i1.n + i2.n,
        .e = i1.e + i2.e,
        .s = i1.s + i2.s,
        .w = i1.w + i2.w
    };
}

static inline bool
ntg_insets_is_zero(ntg_insets insets)
{
    return ((insets.n == 0) && (insets.s == 0) &&
            (insets.e == 0) && (insets.w == 0));
}

static inline bool
ntg_insets_are_eql(ntg_insets insets1, ntg_insets insets2)
{
    return ((insets1.n == insets2.n) &&
            (insets1.e == insets2.e) &&
            (insets1.s == insets2.s) &&
            (insets1.w == insets2.w));
}

/* ------------------------------------------------------ */
/* NTG_SIDE */
/* ------------------------------------------------------ */

static inline enum ntg_orient
ntg_side_get_orient(enum ntg_side side)
{
    if((side == NTG_SIDE_N) || (side == NTG_SIDE_S))
        return NTG_ORIENT_V;
    else
        return NTG_ORIENT_H;
}

/* ------------------------------------------------------ */
/* NTG_ALIGN */
/* ------------------------------------------------------ */

static inline size_t
ntg_align_offset_size(size_t inner_size, size_t outer_size, enum ntg_align align)
{
    if(inner_size > outer_size) inner_size = outer_size;

    if(align == NTG_ALIGN_1)
        return 0;
    else if(align == NTG_ALIGN_2)
        return (outer_size - inner_size) / 2;
    else
        return (outer_size - inner_size);
}

static inline ssize_t
ntg_align_offset_ssize(ssize_t inner_size, ssize_t outer_size, enum ntg_align align)
{
    if(align == NTG_ALIGN_1)
        return 0;
    else if(align == NTG_ALIGN_2)
        return (outer_size - inner_size) / 2;
    else
        return (outer_size - inner_size);
}

#endif // NTG_XY_H
