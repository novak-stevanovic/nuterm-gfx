#ifndef __NTG_CONSTRAIN_CTX_H__
#define __NTG_CONSTRAIN_CTX_H__

#include <stddef.h>

typedef struct ntg_constrain_ctx ntg_constrain_ctx;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

/* Reserved for future use */

/* Child data */
struct ntg_constrain_data
{
    size_t __placeholder;
};

ntg_constrain_ctx* ntg_constrain_ctx_new(size_t capacity, sarena* arena);

struct ntg_constrain_data ntg_constrain_ctx_get(
        const ntg_constrain_ctx* ctx,
        const ntg_drawable* child);
void ntg_constrain_ctx_set(ntg_constrain_ctx* ctx,
        const ntg_drawable* child,
        struct ntg_constrain_data data);

#endif // __NTG_CONSTRAIN_CTX_H__
