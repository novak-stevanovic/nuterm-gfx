#ifndef _NTG_LOOP_CONTEXT_H_
#define _NTG_LOOP_CONTEXT_H_

#include "shared/ntg_xy.h"
#include <stdbool.h>

typedef struct ntg_loop_context ntg_loop_context;
typedef struct ntg_stage ntg_stage;

/* Should not be inherited */
struct ntg_loop_context
{
    ntg_stage *__stage, *__pending_stage;
    bool __loop;
    struct ntg_xy __app_size;

    void* __data;
};

void __ntg_loop_context_init__(
        ntg_loop_context* context,
        ntg_stage* init_stage,
        struct ntg_xy init_app_size,
        void* data);
void __ntg_loop_context_deinit__(ntg_loop_context* context);

void ntg_loop_context_set_stage(ntg_loop_context* context, ntg_stage* stage);
ntg_stage* ntg_loop_context_get_stage(ntg_loop_context* context);
struct ntg_xy ntg_loop_context_get_app_size(const ntg_loop_context* context);
void* ntg_loop_context_get_data(ntg_loop_context* context);

void ntg_loop_context_exit(ntg_loop_context* context);

/* -------------------------------------------------------------------------- */
/* PROTECTED - used by ntg_loop */
/* -------------------------------------------------------------------------- */

bool _ntg_loop_context_update(ntg_loop_context* context);

void _ntg_loop_context_set_app_size(ntg_loop_context* context, struct ntg_xy size);

#endif // _NTG_LOOP_CONTEXT_H_

