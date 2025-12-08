#ifndef __NTG_CONSTRAIN_OUT_H__
#define __NTG_CONSTRAIN_OUT_H__

#include <stddef.h>

typedef struct ntg_constrain_out ntg_constrain_out;
typedef struct ntg_drawable ntg_drawable;
typedef struct sarena sarena;

/* Child result */
struct ntg_constrain_result
{
    size_t size;
};

ntg_constrain_out* ntg_constrain_out_new(size_t capacity, sarena* arena);

struct ntg_constrain_result ntg_constrain_out_get(
        const ntg_constrain_out* out,
        const ntg_drawable* child);
void ntg_constrain_out_set(ntg_constrain_out* out,
        const ntg_drawable* child,
        struct ntg_constrain_result result);

#endif // __NTG_CONSTRAIN_OUT_H__
