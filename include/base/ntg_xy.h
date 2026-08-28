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
    struct
    {
        size_t x, y;
    } ro;
};

GENC_OPT_INLINE_DEF(ntg_xy_opt, ntg_xy)

struct ntg_dxy
{
    struct
    {
        ssize_t x, y;
    } ro;
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

static const ntg_xy NTG_XY_MAX = {
    .ro = { NTG_SIZE_MAX, NTG_SIZE_MAX }
};
static const ntg_xy NTG_XY_ZERO = {0};

static const ntg_dxy NTG_DXY_MAX = {
    .ro = { NTG_SIZE_MAX, NTG_SIZE_MAX }
};
static const ntg_dxy NTG_DXY_ZERO = {0};

enum ntg_dir
{
    NTG_DIR_N = 0,
    NTG_DIR_E,
    NTG_DIR_S,
    NTG_DIR_W
};

struct ntg_insets
{
    struct
    {
        size_t n, e, s, w;
    } ro;
};

static const ntg_insets NTG_INSETS_MAX = {
    .ro = {
        NTG_SIZE_MAX, NTG_SIZE_MAX,
        NTG_SIZE_MAX, NTG_SIZE_MAX
    }
};
static const ntg_insets NTG_INSETS_ZERO = {0};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* NTG_XY */
/* ------------------------------------------------------ */

static inline ntg_xy
ntg_xy_new(size_t x, size_t y)
{
    return (ntg_xy) {
        .ro.x = ntg_min2_size(x, NTG_XY_MAX.ro.x),
        .ro.y = ntg_min2_size(y, NTG_XY_MAX.ro.y),
    };
}

static inline size_t
ntg_xy_x(ntg_xy xy)
{
    return xy.ro.x;
}

static inline size_t
ntg_xy_y(ntg_xy xy)
{
    return xy.ro.y;
}

static inline ntg_xy
ntg_xy_new_orient(size_t prim, size_t sec, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ?
            ntg_xy_new(prim, sec) :
            ntg_xy_new(sec, prim);
}

static inline ntg_xy
ntg_xy_normalize(ntg_xy xy)
{
    if(!xy.ro.x || !xy.ro.y)
        return (ntg_xy) {0};
    else
        return ntg_xy_new(xy.ro.x, xy.ro.y);
}

static inline ntg_xy
ntg_xy_add(ntg_xy a, ntg_xy b)
{
    return (ntg_xy) {
        .ro.x = ntg_min2_size(a.ro.x + b.ro.x, NTG_XY_MAX.ro.x),
        .ro.y = ntg_min2_size(a.ro.y + b.ro.y, NTG_XY_MAX.ro.y),
    };
}

static inline ntg_xy
ntg_xy_sub(ntg_xy a, ntg_xy b)
{
    return (ntg_xy) { 
        .ro.x = ntg_sub2_size(a.ro.x, b.ro.x),
        .ro.y = ntg_sub2_size(a.ro.y, b.ro.y),
    };
}

static inline ntg_xy
ntg_xy_from_dxy(ntg_dxy xy)
{
    return (ntg_xy) {
        .ro.x = ((xy.ro.x >= 0) ? xy.ro.x : 0),
        .ro.y = ((xy.ro.y >= 0) ? xy.ro.y : 0),
    };
}

static inline bool
ntg_xy_is_gt(ntg_xy a, ntg_xy b)
{
    return ((a.ro.x > b.ro.x) && (a.ro.y > b.ro.y));
}

static inline bool
ntg_xy_is_lt(ntg_xy a, ntg_xy b)
{
    return ((a.ro.x < b.ro.x) && (a.ro.y < b.ro.y));
}

static inline bool
ntg_xy_is_le(ntg_xy a, ntg_xy b)
{
    return ((a.ro.x <= b.ro.x) && (a.ro.y <= b.ro.y));
}

static inline bool
ntg_xy_is_ge(ntg_xy a, ntg_xy b)
{
    return ((a.ro.x >= b.ro.x) && (a.ro.y >= b.ro.y));
}

static inline bool
ntg_xy_are_eql(ntg_xy a, ntg_xy b)
{
    return ((a.ro.x == b.ro.x) && (a.ro.y == b.ro.y));
}

static inline ntg_xy
ntg_xy_clamp(ntg_xy min, ntg_xy val, ntg_xy max)
{
    size_t x = val.ro.x;
    size_t y = val.ro.y;

    if(x < min.ro.x) x = min.ro.x;
    else if(x > max.ro.x) x = max.ro.x;

    if(y < min.ro.y) y = min.ro.y;
    else if(y > max.ro.y) y = max.ro.y;

    return (ntg_xy) { .ro.x = x, .ro.y = y };
}

static inline bool
ntg_xy_is_in_rect(ntg_xy pos, ntg_xy start, ntg_xy end)
{
    return ((pos.ro.x >= start.ro.x) && (pos.ro.y >= start.ro.y) &&
            (pos.ro.x < end.ro.x) && (pos.ro.y < end.ro.y));
}

static inline bool
ntg_xy_is_zero(ntg_xy xy)
{
    return ((xy.ro.x == 0) && (xy.ro.y == 0));
}

static inline bool
ntg_xy_is_zero_any(ntg_xy xy)
{
    return ((xy.ro.x == 0) || (xy.ro.y == 0));
}

static inline ntg_xy
ntg_xy_transpose(ntg_xy xy)
{
    return (ntg_xy) {
        .ro.x = xy.ro.y,
        .ro.y = xy.ro.x
    };
}

static inline size_t
ntg_xy_get(ntg_xy xy, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? xy.ro.x : xy.ro.y;
}

static inline size_t
ntg_xy_get_other(ntg_xy xy, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_V) ? xy.ro.x : xy.ro.y;
}

static inline ntg_xy
ntg_xy_set(ntg_xy xy, size_t val, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ?
            ntg_xy_new(val, xy.ro.y) :
            ntg_xy_new(xy.ro.x, val);
}

static inline ntg_xy
ntg_xy_set_other(ntg_xy xy, size_t val, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_V) ?
            ntg_xy_new(val, xy.ro.y) :
            ntg_xy_new(xy.ro.x, val);
}

static inline ntg_xy
ntg_xy_set_x(ntg_xy xy, size_t x)
{
    return ntg_xy_new(x, xy.ro.y);
}

static inline ntg_xy
ntg_xy_set_y(ntg_xy xy, size_t y)
{
    return ntg_xy_new(xy.ro.x, y);
}

static inline ntg_xy
ntg_xy_add_x(ntg_xy xy, size_t x_add)
{
    return ntg_xy_add(xy, ntg_xy_new(x_add, 0));
}

static inline ntg_xy
ntg_xy_add_y(ntg_xy xy, size_t y_add)
{
    return ntg_xy_add(xy, ntg_xy_new(0, y_add));
}

static inline ntg_xy
ntg_xy_sub_x(ntg_xy xy, size_t x_sub)
{
    return ntg_xy_sub(xy, ntg_xy_new(x_sub, 0));
}

static inline ntg_xy
ntg_xy_sub_y(ntg_xy xy, size_t y_sub)
{
    return ntg_xy_sub(xy, ntg_xy_new(0, y_sub));
}

static inline ntg_xy
ntg_xy_pos_clamp(ntg_xy pos, ntg_xy size, ntg_xy parent_size)
{
    size_t x = (size.ro.x >= parent_size.ro.x) ?
            0 : ntg_min2_size(pos.ro.x, parent_size.ro.x - size.ro.x);
    size_t y = (size.ro.y >= parent_size.ro.y) ?
            0 : ntg_min2_size(pos.ro.y, parent_size.ro.y - size.ro.y);

    return (ntg_xy) { .ro.x = x, .ro.y = y };
}

/* ------------------------------------------------------ */
/* NTG_DXY */
/* ------------------------------------------------------ */

static inline ntg_dxy
ntg_dxy_new(ssize_t x, ssize_t y)
{
    return (ntg_dxy) {
        .ro.x = ntg_min2_ssize(x, NTG_DXY_MAX.ro.x),
        .ro.y = ntg_min2_ssize(y, NTG_DXY_MAX.ro.y),
    };
}

static inline ssize_t
ntg_dxy_x(ntg_dxy dxy)
{
    return dxy.ro.x;
}

static inline ssize_t
ntg_dxy_y(ntg_dxy dxy)
{
    return dxy.ro.y;
}

static inline ntg_dxy
ntg_dxy_new_orient(ssize_t prim, ssize_t sec, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ?
            ntg_dxy_new(prim, sec) :
            ntg_dxy_new(sec, prim);
}

static inline ntg_dxy
ntg_dxy_normalize(ntg_dxy dxy)
{
    if(!dxy.ro.x || !dxy.ro.y)
        return (ntg_dxy) {0};
    else
        return ntg_dxy_new(dxy.ro.x, dxy.ro.y);
}

static inline ntg_dxy
ntg_dxy_add(ntg_dxy a, ntg_dxy b)
{
    return (ntg_dxy) {
        .ro.x = ntg_min2_ssize(a.ro.x + b.ro.x, NTG_DXY_MAX.ro.x),
        .ro.y = ntg_min2_ssize(a.ro.y + b.ro.y, NTG_DXY_MAX.ro.y),
    };
}

static inline ntg_dxy
ntg_dxy_sub(ntg_dxy a, ntg_dxy b)
{
    return (ntg_dxy) {
        .ro.x = a.ro.x - b.ro.x,
        .ro.y = a.ro.y - b.ro.y
    };
}

static inline ntg_dxy
ntg_dxy_from_xy(ntg_xy xy)
{
    return (ntg_dxy) {
        .ro.x = xy.ro.x,
        .ro.y = xy.ro.y
    };
}

static inline bool
ntg_dxy_is_gt(ntg_dxy a, ntg_dxy b)
{
    return ((a.ro.x > b.ro.x) && (a.ro.y > b.ro.y));
}

static inline bool
ntg_dxy_is_lt(ntg_dxy a, ntg_dxy b)
{
    return ((a.ro.x < b.ro.x) && (a.ro.y < b.ro.y));
}

static inline bool
ntg_dxy_is_le(ntg_dxy a, ntg_dxy b)
{
    return ((a.ro.x <= b.ro.x) && (a.ro.y <= b.ro.y));
}

static inline bool
ntg_dxy_is_ge(ntg_dxy a, ntg_dxy b)
{
    return ((a.ro.x >= b.ro.x) && (a.ro.y >= b.ro.y));
}

static inline bool
ntg_dxy_are_eql(ntg_dxy a, ntg_dxy b)
{
    return ((a.ro.x == b.ro.x) && (a.ro.y == b.ro.y));
}

static inline ntg_dxy
ntg_dxy_clamp(ntg_dxy min, ntg_dxy val, ntg_dxy max)
{
    ssize_t x = val.ro.x;
    ssize_t y = val.ro.y;

    if(x < min.ro.x) x = min.ro.x;
    else if(x > max.ro.x) x = max.ro.x;

    if(y < min.ro.y) y = min.ro.y;
    else if(y > max.ro.y) y = max.ro.y;

    return (ntg_dxy) { .ro.x = x, .ro.y = y };
}

static inline bool
ntg_dxy_is_in_rect(ntg_dxy pos, ntg_dxy start, ntg_dxy end)
{
    return ((pos.ro.x >= start.ro.x) && (pos.ro.y >= start.ro.y) &&
            (pos.ro.x < end.ro.x) && (pos.ro.y < end.ro.y));
}

static inline bool
ntg_dxy_is_zero(ntg_dxy dxy)
{
    return ((dxy.ro.x == 0) && (dxy.ro.y == 0));
}

static inline bool
ntg_dxy_is_zero_any(ntg_dxy dxy)
{
    return ((dxy.ro.x == 0) || (dxy.ro.y == 0));
}

static inline ntg_dxy
ntg_dxy_transpose(ntg_dxy xy)
{
    return (ntg_dxy) {
        .ro.x = xy.ro.y,
        .ro.y = xy.ro.x
    };
}

static inline ssize_t
ntg_dxy_get(ntg_dxy xy, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? xy.ro.x : xy.ro.y;
}

static inline ssize_t
ntg_dxy_get_other(ntg_dxy xy, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_V) ? xy.ro.x : xy.ro.y;
}

static inline ntg_dxy
ntg_dxy_set(ntg_dxy dxy, ssize_t val, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ?
            ntg_dxy_new(val, dxy.ro.y) :
            ntg_dxy_new(dxy.ro.x, val);
}

static inline ntg_dxy
ntg_dxy_set_other(ntg_dxy dxy, ssize_t val, enum ntg_orient orient)
{
    return (orient == NTG_ORIENT_V) ?
            ntg_dxy_new(val, dxy.ro.y) :
            ntg_dxy_new(dxy.ro.x, val);
}

static inline ntg_dxy
ntg_dxy_set_x(ntg_dxy dxy, ssize_t x)
{
    return ntg_dxy_new(x, dxy.ro.y);
}

static inline ntg_dxy
ntg_dxy_set_y(ntg_dxy dxy, ssize_t y)
{
    return ntg_dxy_new(dxy.ro.x, y);
}

static inline ntg_dxy
ntg_dxy_add_x(ntg_dxy dxy, ssize_t x_add)
{
    return ntg_dxy_add(dxy, ntg_dxy_new(x_add, 0));
}

static inline ntg_dxy
ntg_dxy_add_y(ntg_dxy dxy, ssize_t y_add)
{
    return ntg_dxy_add(dxy, ntg_dxy_new(0, y_add));
}

static inline ntg_dxy
ntg_dxy_sub_x(ntg_dxy dxy, ssize_t x_sub)
{
    return ntg_dxy_sub(dxy, ntg_dxy_new(x_sub, 0));
}

static inline ntg_dxy
ntg_dxy_sub_y(ntg_dxy dxy, ssize_t y_sub)
{
    return ntg_dxy_sub(dxy, ntg_dxy_new(0, y_sub));
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
    return (ntg_insets) { 
        .ro.n = ntg_min2_size(n, NTG_INSETS_MAX.ro.n),
        .ro.e = ntg_min2_size(e, NTG_INSETS_MAX.ro.e),
        .ro.s = ntg_min2_size(s, NTG_INSETS_MAX.ro.s),
        .ro.w = ntg_min2_size(w, NTG_INSETS_MAX.ro.w),
    };
}

static inline size_t
ntg_insets_n(ntg_insets insets)
{
    return insets.ro.n;
}

static inline size_t
ntg_insets_e(ntg_insets insets)
{
    return insets.ro.e;
}

static inline size_t
ntg_insets_s(ntg_insets insets)
{
    return insets.ro.s;
}

static inline size_t
ntg_insets_w(ntg_insets insets)
{
    return insets.ro.w;
}

static inline size_t
ntg_insets_hsum(ntg_insets insets)
{
    return ntg_min2_size(NTG_SIZE_MAX, insets.ro.e + insets.ro.w);
}

static inline size_t
ntg_insets_vsum(ntg_insets insets)
{
    return ntg_min2_size(NTG_SIZE_MAX, insets.ro.n + insets.ro.s);
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
        .ro.n = ntg_min2_size(i1.ro.n + i2.ro.n, NTG_INSETS_MAX.ro.n),
        .ro.e = ntg_min2_size(i1.ro.e + i2.ro.e, NTG_INSETS_MAX.ro.e),
        .ro.s = ntg_min2_size(i1.ro.s + i2.ro.s, NTG_INSETS_MAX.ro.s),
        .ro.w = ntg_min2_size(i1.ro.w + i2.ro.w, NTG_INSETS_MAX.ro.w)
    };
}

static inline ntg_insets
ntg_insets_set(ntg_insets insets, size_t val, enum ntg_dir dir)
{
    switch(dir)
    {
        case NTG_DIR_N:
            return ntg_insets_new(val, insets.ro.e, insets.ro.s, insets.ro.w);
        case NTG_DIR_E:
            return ntg_insets_new(insets.ro.n, val, insets.ro.s, insets.ro.w);
        case NTG_DIR_S:
            return ntg_insets_new(insets.ro.n, insets.ro.e, val, insets.ro.w);
        case NTG_DIR_W:
            return ntg_insets_new(insets.ro.n, insets.ro.e, insets.ro.s, val);
        default:
            return insets;
    }
}

static inline size_t
ntg_insets_get(ntg_insets insets, enum ntg_dir dir)
{
    switch(dir)
    {
        case NTG_DIR_N:
            return insets.ro.n;
        case NTG_DIR_E:
            return insets.ro.e;
        case NTG_DIR_S:
            return insets.ro.s;
        case NTG_DIR_W:
            return insets.ro.w;
    }
}

static inline bool
ntg_insets_is_zero(ntg_insets insets)
{
    return ((insets.ro.n == 0) && (insets.ro.s == 0) &&
            (insets.ro.e == 0) && (insets.ro.w == 0));
}

static inline bool
ntg_insets_are_eql(ntg_insets insets1, ntg_insets insets2)
{
    return ((insets1.ro.n == insets2.ro.n) &&
            (insets1.ro.e == insets2.ro.e) &&
            (insets1.ro.s == insets2.ro.s) &&
            (insets1.ro.w == insets2.ro.w));
}

/* ------------------------------------------------------ */
/* NTG_SIDE */
/* ------------------------------------------------------ */

static inline enum ntg_orient
ntg_side_get_orient(enum ntg_dir side)
{
    if((side == NTG_DIR_N) || (side == NTG_DIR_S))
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
