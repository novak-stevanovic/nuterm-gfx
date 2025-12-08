#ifndef __NTG_ARRANGE_OUT_H__
#define __NTG_ARRANGE_OUT_H__

#include "shared/ntg_xy.h"

typedef struct ntg_arrange_out ntg_arrange_out;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

struct ntg_arrange_result
{
    struct ntg_xy pos;
};

ntg_arrange_out* ntg_arrange_out_new(size_t capacity, sarena* arena);

struct ntg_arrange_result ntg_arrange_out_get(
        const ntg_arrange_out* out,
        const ntg_drawable* child);
void ntg_arrange_out_set(ntg_arrange_out* out,
        const ntg_drawable* child,
        struct ntg_arrange_result result);

#endif // __NTG_ARRANGE_OUT_H__
