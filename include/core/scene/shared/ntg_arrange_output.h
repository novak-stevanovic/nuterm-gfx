#ifndef _NTG_ARRANGE_OUTPUT_H_
#define _NTG_ARRANGE_OUTPUT_H_

#include "shared/ntg_xy.h"

typedef struct ntg_arrange_output ntg_arrange_output;
typedef struct ntg_drawable ntg_drawable;

struct ntg_arrange_result
{
    struct ntg_xy pos;
};

ntg_arrange_output* ntg_arrange_output_new();
void ntg_arrange_output_destroy(ntg_arrange_output* output);

void ntg_arrange_output_set(ntg_arrange_output* output,
        const ntg_drawable* child,
        struct ntg_arrange_result result);
struct ntg_arrange_result ntg_arrange_output_get(
        const ntg_arrange_output* output,
        const ntg_drawable* child);

#endif // _NTG_ARRANGE_OUTPUT_H_
