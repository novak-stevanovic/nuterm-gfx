#ifndef _NTG_CONSTRAIN_CONTEXT_H_
#define _NTG_CONSTRAIN_CONTEXT_H_

#include <stddef.h>

/* Child data */
struct ntg_constrain_data
{
    size_t min_size, natural_size,
           max_size, grow;
};

typedef struct ntg_drawable ntg_drawable;

typedef struct ntg_constrain_context ntg_constrain_context;

ntg_constrain_context* ntg_constrain_context_new();
void ntg_constrain_context_destroy(ntg_constrain_context* context);

struct ntg_constrain_data ntg_constrain_context_get(
        const ntg_constrain_context* context,
        const ntg_drawable* child);
void ntg_constrain_context_set(ntg_constrain_context* context,
        const ntg_drawable* child,
        struct ntg_constrain_data data);

#endif // _NTG_CONSTRAIN_CONTEXT_H_
