#ifndef __NTG_CONSTRAIN_OUTPUT_H__
#define __NTG_CONSTRAIN_OUTPUT_H__

#include <stddef.h>

typedef struct ntg_constrain_output ntg_constrain_output;
typedef struct ntg_drawable ntg_drawable;

/* Child result */
struct ntg_constrain_result
{
    size_t size;
};

ntg_constrain_output* ntg_constrain_output_new();
void ntg_constrain_output_destroy(ntg_constrain_output* output);

void ntg_constrain_output_set(ntg_constrain_output* output,
        const ntg_drawable* child,
        struct ntg_constrain_result result);
struct ntg_constrain_result ntg_constrain_output_get(
        const ntg_constrain_output* output,
        const ntg_drawable* child);

#endif // __NTG_CONSTRAIN_OUTPUT_H__
