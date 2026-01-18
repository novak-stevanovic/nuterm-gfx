#ifndef NTG_XY_H
#define NTG_XY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include "shared/ntg_typedef.h"

#define NTG_SIZE_MAX 10000

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
    /* left, when used in reference to x axis align */
    /* top, when used in reference to y axis align */
    NTG_ALIGN_1,

    /* center */
    NTG_ALIGN_2,

    /* right, when used in reference to x axis align */
    /* bottom, when used in reference to y axis align */
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

/* -------------------------------------------------------------------------- */

static inline struct ntg_xy ntg_xy(size_t x, size_t y)
{
    return (struct ntg_xy) { .x = x, .y = y };
}

static inline void ntg_xy_size_(struct ntg_xy* xy)
{
    if(xy == NULL) return;

    if((xy->x == 0) || (xy->y == 0))
    {
        xy->x = 0;
        xy->y = 0;
    }
}

static inline struct ntg_xy ntg_xy_size(struct ntg_xy xy)
{
    ntg_xy_size_(&xy);
    return xy;
}

static inline struct ntg_xy ntg_xy_add(struct ntg_xy a, struct ntg_xy b)
{
    return (struct ntg_xy) { .x = a.x + b.x, .y = a.y + b.y };
}

static inline struct ntg_xy ntg_xy_sub(struct ntg_xy a, struct ntg_xy b)
{
    return (struct ntg_xy) { 
        .x = (a.x > b.x) ? (a.x - b.x) : 0,
        .y = (a.y > b.y) ? (a.y - b.y) : 0,
    };
}

static inline struct ntg_xy ntg_xy_from_dxy(struct ntg_dxy xy)
{
    return (struct ntg_xy) { .x = (size_t)xy.x, .y = (size_t)xy.y };
}

static inline bool ntg_xy_is_greater(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x > b.x) && (a.y > b.y));
}

static inline bool ntg_xy_is_lesser(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x < b.x) && (a.y < b.y));
}

static inline bool ntg_xy_is_greater_eq(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x >= b.x) && (a.y >= b.y));
}

static inline bool ntg_xy_are_equal(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x == b.x) && (a.y == b.y));
}

static inline struct ntg_xy ntg_xy_clamp(struct ntg_xy min, struct ntg_xy val,
        struct ntg_xy max)
{
    if(val.x < min.x) val.x = min.x;
    else if(val.x > max.x) val.x = max.x;

    if(val.y < min.y) val.y = min.y;
    else if(val.y > max.y) val.y = max.y;

    return val;
}

static inline bool ntg_xy_is_zero(struct ntg_xy size)
{
    return ((size.x == 0) && (size.y == 0));
}

static inline bool ntg_xy_size_is_zero(struct ntg_xy size)
{
    return ((size.x == 0) || (size.y == 0));
}

static inline struct ntg_xy ntg_xy_transpose(struct ntg_xy xy)
{
    return (struct ntg_xy) {
        .x = xy.y,
        .y = xy.x
    };
}

static inline size_t ntg_xy_get(struct ntg_xy xy, ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? xy.x : xy.y;
}

/* -------------------------------------------------------------------------- */

static inline struct ntg_dxy ntg_dxy(ssize_t x, ssize_t y)
{
    return (struct ntg_dxy) { .x = x, .y = y };
}

static inline struct ntg_dxy ntg_dxy_from_xy(struct ntg_xy xy)
{
    return (struct ntg_dxy) { .x = (ssize_t)xy.x, .y = (ssize_t)xy.y };
}

static inline struct ntg_dxy ntg_dxy_add(struct ntg_dxy a, struct ntg_dxy b)
{
    return (struct ntg_dxy) { .x = a.x + b.x, .y = a.y + b.y };
}

static inline struct ntg_dxy ntg_dxy_sub(struct ntg_dxy a, struct ntg_dxy b)
{
    return (struct ntg_dxy) { .x = a.x - b.x, .y = a.y - b.y };
}

static inline struct ntg_dxy ntg_dxy_clamp(struct ntg_dxy min, struct ntg_dxy val,
        struct ntg_dxy max)
{
    if(val.x < min.x) val.x = min.x;
    else if(val.x > max.x) val.x = max.x;

    if(val.y < min.y) val.y = min.y;
    else if(val.y > max.y) val.y = max.y;

    return val;
}

static inline size_t ntg_dxy_get(struct ntg_dxy xy, ntg_orient orient)
{
    return (orient == NTG_ORIENT_H) ? xy.x : xy.y;
}

/* -------------------------------------------------------------------------- */

static inline ntg_orient ntg_orient_get_other(ntg_orient ort)
{
    return (ort == NTG_ORIENT_H) ?
        NTG_ORIENT_V :
        NTG_ORIENT_H;
}

/* -------------------------------------------------------------------------- */

static inline struct ntg_xy ntg_xy_from_oxy(struct ntg_oxy orient_xy)
{
    if(orient_xy.orient == NTG_ORIENT_H)
        return ntg_xy(orient_xy.prim_val, orient_xy.sec_val);
    else
        return ntg_xy(orient_xy.sec_val, orient_xy.prim_val);
}

static inline struct ntg_oxy ntg_oxy(size_t prim_val, size_t sec_val,
        ntg_orient orient)
{
    return (struct ntg_oxy) {
        .prim_val = prim_val,
        .sec_val = sec_val,
        .orient = orient
    };
}

static inline struct ntg_oxy ntg_oxy_size(struct ntg_oxy oxy)
{
    if((oxy.prim_val == 0) || (oxy.sec_val == 0))
        return ntg_oxy(0, 0, oxy.orient);
    else
        return oxy;
}

static inline struct ntg_oxy ntg_oxy_from_xy(struct ntg_xy xy,
        ntg_orient orient)
{
    if(orient == NTG_ORIENT_H)
        return ntg_oxy(xy.x, xy.y, NTG_ORIENT_H);
    else
        return ntg_oxy(xy.y, xy.x, NTG_ORIENT_V);
}

/* -------------------------------------------------------------------------- */

#endif // NTG_XY_H
