#ifndef __NTG_ARRANGE_CONTEXT_H__
#define __NTG_ARRANGE_CONTEXT_H__

#include "shared/ntg_xy.h"

typedef struct ntg_arrange_context ntg_arrange_context;
typedef struct ntg_drawable ntg_drawable;

/* Child data */
struct ntg_arrange_data
{
    struct ntg_xy size;
};

ntg_arrange_context* ntg_arrange_context_new();
void ntg_arrange_context_destroy(ntg_arrange_context* context);

struct ntg_arrange_data ntg_arrange_context_get(
        const ntg_arrange_context* context,
        const ntg_drawable* child);
void ntg_arrange_context_set(ntg_arrange_context* context,
        const ntg_drawable* child,
        struct ntg_arrange_data data);

#endif // __NTG_ARRANGE_CONTEXT_H__
