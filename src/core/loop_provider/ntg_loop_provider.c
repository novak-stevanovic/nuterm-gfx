#include <stdlib.h>
#include <assert.h>

#include "core/loop_provider/ntg_loop_provider.h"

void __ntg_loop_provider_init__(
        ntg_loop_provider* loop_provider,
        ntg_loop_provider_run_fn run_fn,
        ntg_stage* init_stage)
{
    assert(loop_provider != NULL);
    assert(run_fn != NULL);
    assert(init_stage != NULL);

    loop_provider->__stage = init_stage;
    loop_provider->__next_stage = init_stage;
    loop_provider->__run_fn = run_fn;
    loop_provider->__loop = false;
}

void __ntg_loop_provider_deinit__(ntg_loop_provider* loop_provider)
{
    assert(loop_provider != NULL);

    loop_provider->__stage = NULL;
    loop_provider->__run_fn = NULL;
}

void ntg_loop_provider_run(ntg_loop_provider* loop_provider)
{
    assert(loop_provider != NULL);

    loop_provider->__loop = true;
    loop_provider->__run_fn(loop_provider);
}

void ntg_loop_provider_stop(ntg_loop_provider* loop_provider)
{
    assert(loop_provider != NULL);

    loop_provider->__loop = false;
}

ntg_stage* ntg_loop_provider_get_stage(ntg_loop_provider* loop_provider)
{
    assert(loop_provider != NULL);

    return loop_provider->__stage;
}

void ntg_loop_provider_set_stage(ntg_loop_provider* loop_provider,
        ntg_stage* stage)
{
    assert(loop_provider != NULL);

    loop_provider->__next_stage = stage;
}
