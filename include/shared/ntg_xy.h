#ifndef _NTG_XY_H_
#define _NTG_XY_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define NTG_TERM_MAX_WIDTH 500
#define NTG_TERM_MAX_HEIGHT 150

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

static const struct ntg_xy NTG_XY_UNSET = {0};

static const struct ntg_dxy NTG_DXY_UNSET = {0};

static const struct ntg_constr NTG_CONSTR_UNSET = {
    .min_size = { 0, 0 },
    .max_size = { SIZE_MAX, SIZE_MAX }
};

/* -------------------------------------------------------------------------- */

static inline struct ntg_xy ntg_xy(size_t x, size_t y)
{
    return (struct ntg_xy) { .x = x, .y = y };
}

static inline struct ntg_xy ntg_xy_add(struct ntg_xy a, struct ntg_xy b)
{
    return (struct ntg_xy) { .x = a.x + b.x, .y = a.y + b.y };
}

static inline struct ntg_dxy ntg_xy_sub(struct ntg_xy a, struct ntg_xy b)
{
    return (struct ntg_dxy) { .x = a.x + b.x, .y = a.y + b.y };
}

static inline struct ntg_xy ntg_xy_from_dxy(struct ntg_dxy xy)
{
    return (struct ntg_xy) { .x = xy.x, .y = xy.y };
}

static inline bool ntg_xy_is_greater(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x > b.x) && (a.y > b.y));
}

static inline bool ntg_xy_are_equal(struct ntg_xy a, struct ntg_xy b)
{
    return ((a.x == b.x) && (a.y == b.y));
}

/* -------------------------------------------------------------------------- */

static inline struct ntg_xy ntg_dxy(ssize_t x, ssize_t y)
{
    return (struct ntg_xy) { .x = x, .y = y };
}

static inline struct ntg_dxy ntg_dxy_from_xy(struct ntg_xy xy)
{
    return (struct ntg_dxy) { .x = xy.x, .y = xy.y };
}

/* -------------------------------------------------------------------------- */

static inline struct ntg_constr ntg_constr(struct ntg_xy min_size,
        struct ntg_xy max_size)
{
    return (struct ntg_constr) { .min_size = min_size, .max_size = max_size };
}

#endif // _NTG_XY_H_
