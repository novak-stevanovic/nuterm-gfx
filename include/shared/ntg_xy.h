#ifndef _NTG_XY_H_
#define _NTG_XY_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define NTG_TERM_MAX_WIDTH 300
#define NTG_TERM_MAX_HEIGHT 100

struct ntg_xy
{
    size_t x, y;
};

struct ntg_dxy
{
    ssize_t x, y;
};

#define NTG_XY(xv, yv) ((struct ntg_xy) { .x = (xv), .y = (yv) })
#define NTG_XY_ADD(a, b) ((struct ntg_xy) { .x = (a).x + (b).x, .y = (a).y + (b).y })
#define NTG_XY_SUB(a, b) ((struct ntg_dxy) {                                   \
        .x = ((ssize_t)(a).x - (b).x),                                         \
        .y = ((ssize_t)(a).y - (b).y) })

#define NTG_XY_DXY(xy) ((struct ntg_dxy) { .x = (xy).x, .y = (xy).y })
#define NTG_DXY_XY(xy) ((struct ntg_xy) { .x = (xy).x, .y = (xy).y })

#define NTG_XY_SIZE_UNSET ((struct ntg_xy) {0})
#define NTG_XY_POS_UNSET ((struct ntg_xy) { .x = SIZE_MAX, .y = SIZE_MAX })
#define NTG_XY_POS_IN_BOUNDS(pos,bounds) ( (((pos).x < (bounds).x) && ((pos).y < (bounds).y)) )

struct ntg_constr
{
    struct ntg_xy min_size, max_size;
};

#define NTG_CONSTR(minsz, maxsz) ((struct ntg_constr) {                        \
    .min_size = (minsz),                                                       \
    .max_size = (maxsz)                                                        \
})
#define NTG_CONSTR_UNSET NTG_CONSTR(NTG_XY(0, 0), NTG_XY(SIZE_MAX, SIZE_MAX))

#endif // _NTG_XY_H_
