#ifndef _NTG_MEASURE_CONTEXT_H_
#define _NTG_MEASURE_CONTEXT_H_

#include <stddef.h>

typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_measure_context ntg_measure_context;

/* Child data */

struct ntg_measure_data
{
    size_t min_size, natural_size, max_size, grow;
};

ntg_measure_context* ntg_measure_context_new();
void ntg_measure_context_destroy(ntg_measure_context* context);

struct ntg_measure_data ntg_measure_context_get(
        const ntg_measure_context* context,
        const ntg_drawable* child);
void ntg_measure_context_set(
        ntg_measure_context* context,
        const ntg_drawable* child,
        struct ntg_measure_data data);

#endif // _NTG_MEASURE_CONTEXT_H_
