#ifndef __NTG_ARRANGE_CTX_H__
#define __NTG_ARRANGE_CTX_H__

#include "shared/ntg_xy.h"

typedef struct ntg_arrange_ctx ntg_arrange_ctx;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

/* Child data */
struct ntg_arrange_data
{
    struct ntg_xy size;
};

ntg_arrange_ctx* ntg_arrange_ctx_new(size_t capacity, sarena* arena);

struct ntg_arrange_data ntg_arrange_ctx_get(
        const ntg_arrange_ctx* ctx,
        const ntg_drawable* child);
void ntg_arrange_ctx_set(ntg_arrange_ctx* ctx,
        const ntg_drawable* child,
        struct ntg_arrange_data data);

#endif // __NTG_ARRANGE_CTX_H__
