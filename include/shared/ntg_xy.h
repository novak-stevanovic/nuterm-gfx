#ifndef _NTG_XY_H_
#define _NTG_XY_H_

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define NTG_SIZE_MAX 10000

struct ntg_xy
{
    size_t x, y;
};

struct ntg_dxy
{
    ssize_t x, y;
};

struct ntg_constr
{
    struct ntg_xy min_size, max_size;
};

typedef enum ntg_orientation
{
    NTG_ORIENTATION_HORIZONTAL,
    NTG_ORIENTATION_VERTICAL
} ntg_orientation;

typedef enum ntg_alignment
{
    /* left, when used in reference to x axis alignment */
    /* top, when used in reference to y axis alignment */
    NTG_ALIGNMENT_1,

    /* center */
    NTG_ALIGNMENT_2,

    /* right, when used in reference to x axis alignment */
    /* bottom, when used in reference to y axis alignment */
    NTG_ALIGNMENT_3

} ntg_alignment;

struct ntg_oxy
{
    size_t prim_val, sec_val;
};

static const struct ntg_xy NTG_XY_MAX = { NTG_SIZE_MAX, NTG_SIZE_MAX };
static const struct ntg_xy NTG_XY_MIN = { 0, 0 };
static const struct ntg_xy NTG_XY_UNSET = { 0, 0 };

static const struct ntg_dxy NTG_DXY_MAX = { NTG_SIZE_MAX, NTG_SIZE_MAX };
static const struct ntg_dxy NTG_DXY_MIN = { 0, 0 };
static const struct ntg_dxy NTG_DXY_UNSET = { 0, 0 };

static const struct ntg_constr NTG_CONSTR_UNSET = {
    .min_size = { 0, 0 },
    .max_size = { SIZE_MAX, SIZE_MAX }
};

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

/* -------------------------------------------------------------------------- */

static inline struct ntg_constr ntg_constr(struct ntg_xy min_size,
        struct ntg_xy max_size)
{
    return (struct ntg_constr) { .min_size = min_size, .max_size = max_size };
}

static inline bool ntg_constr_contains(struct ntg_constr constr,
        struct ntg_xy point)
{
    return (point.x >= constr.min_size.x) && (point.x < constr.max_size.x) &&
        (point.y >= constr.min_size.y) && (point.y < constr.max_size.y);
}

static inline bool ntg_constr_are_equal(struct ntg_constr c1, struct ntg_constr c2)
{
    return (ntg_xy_are_equal(c1.min_size, c2.min_size) &&
            ntg_xy_are_equal(c1.max_size, c2.max_size));
}

/* -------------------------------------------------------------------------- */

static inline ntg_orientation ntg_orientation_get_other(ntg_orientation ort)
{
    return (ort == NTG_ORIENTATION_HORIZONTAL) ?
        NTG_ORIENTATION_VERTICAL :
        NTG_ORIENTATION_HORIZONTAL;
}

/* -------------------------------------------------------------------------- */

static inline struct ntg_xy ntg_xy_from_oxy(struct ntg_oxy orient_xy,
        ntg_orientation orientation)
{
    if(orientation == NTG_ORIENTATION_HORIZONTAL)
        return ntg_xy(orient_xy.prim_val, orient_xy.sec_val);
    else
        return ntg_xy(orient_xy.sec_val, orient_xy.prim_val);
}

static inline struct ntg_oxy ntg_oxy(size_t prim_val, size_t sec_val)
{
    return (struct ntg_oxy) {
        .prim_val = prim_val,
        .sec_val = sec_val
    };
}

static inline struct ntg_oxy ntg_oxy_size(struct ntg_oxy oxy)
{
    if((oxy.prim_val == 0) || (oxy.sec_val == 0))
        return ntg_oxy(0, 0);
    else
        return oxy;
}

static inline struct ntg_oxy ntg_oxy_from_xy(struct ntg_xy xy,
        ntg_orientation orientation)
{
    if(orientation == NTG_ORIENTATION_HORIZONTAL)
        return ntg_oxy(xy.x, xy.y);
    else
        return ntg_oxy(xy.y, xy.x);
}

/* -------------------------------------------------------------------------- */

#endif // _NTG_XY_H_
