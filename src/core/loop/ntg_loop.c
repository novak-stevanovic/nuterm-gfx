#include <assert.h>

#include "core/loop/ntg_loop.h"
#include "core/loop/ntg_loop_context.h"

void __ntg_loop_init__(ntg_loop* loop,
        ntg_loop_fn run_fn,
        ntg_stage* init_stage)
{
    assert(loop != NULL);
    assert(run_fn != NULL);
    assert(init_stage != NULL);

    loop->__run_fn = run_fn;
    loop->__init_stage = init_stage;
}

void __ntg_loop_deinit__(ntg_loop* loop)
{
    assert(loop != NULL);

    loop->__run_fn = NULL;
    loop->__init_stage = NULL;
}

void ntg_loop_run(ntg_loop* loop)
{
    assert(loop != NULL);

    ntg_loop_context context;
    __ntg_loop_context_init__(&context, loop->__init_stage);

    struct ntg_loop_status it_status;
    while(_ntg_loop_context_update(&context))
    {
        it_status = loop->__run_fn(&context);
        
        // TODO: raise event
    }

    __ntg_loop_context_deinit__(&context);
}
