#ifndef _NTG_LOOP_CONTEXT_H_
#define _NTG_LOOP_CONTEXT_H_

#include <stdbool.h>

typedef struct ntg_loop_context ntg_loop_context;
typedef struct ntg_stage ntg_stage;

struct ntg_loop_context
{
    ntg_stage *__current, *__next;
    bool __loop;
};

void __ntg_loop_context_init__(ntg_loop_context* context, ntg_stage* init_stage);
void __ntg_loop_context_deinit__(ntg_loop_context* context);

void ntg_loop_context_set_stage(ntg_loop_context* context, ntg_stage* stage);
ntg_stage* ntg_loop_context_get_stage(ntg_loop_context* context);

void ntg_loop_context_exit(ntg_loop_context* context);

/* Used by ntg_loop */
bool _ntg_loop_context_update(ntg_loop_context* context);

#endif // _NTG_LOOP_CONTEXT_H_

