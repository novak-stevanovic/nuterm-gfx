#ifndef __NTG_MEASURE_CTX_H__
#define __NTG_MEASURE_CTX_H__

#include <stddef.h>

typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_measure_ctx ntg_measure_ctx;
typedef struct sarena sarena;

/* Child data */

struct ntg_measure_data
{
    size_t min_size, natural_size,
           max_size, grow;
};

ntg_measure_ctx* ntg_measure_ctx_new(size_t capacity, sarena* arena);

struct ntg_measure_data ntg_measure_ctx_get(
        const ntg_measure_ctx* ctx,
        const ntg_drawable* child);
void ntg_measure_ctx_set(ntg_measure_ctx* ctx,
        const ntg_drawable* child,
        struct ntg_measure_data data);

#endif // __NTG_MEASURE_CTX_H__
