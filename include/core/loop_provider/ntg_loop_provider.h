#ifndef _NTG_LOOP_PROVIDER_H_
#define _NTG_LOOP_PROVIDER_H_

#include <stdbool.h>

typedef struct ntg_loop_provider ntg_loop_provider;
typedef struct ntg_stage ntg_stage;

// TODO: Rename -> ntg_loop
// TODO: Introduce ntg_loop_context

/* This function should start a loop.
 *
 * The loop must update `__stage` on each iteration based on `__next_stage`.
 * The loop must check if it should continue on each iteration. */
typedef void (*ntg_loop_provider_run_fn)(ntg_loop_provider* loop_provider);

struct ntg_loop_provider
{
    ntg_stage* __stage;
    ntg_stage* __next_stage;

    bool __loop;

    ntg_loop_provider_run_fn __run_fn;
};

void __ntg_loop_provider_init__(
        ntg_loop_provider* loop_provider,
        ntg_loop_provider_run_fn run_fn,
        ntg_stage* init_stage);
void __ntg_loop_provider_deinit__(ntg_loop_provider* loop_provider);

void ntg_loop_provider_run(ntg_loop_provider* loop_provider);
void ntg_loop_provider_stop(ntg_loop_provider* loop_provider);

ntg_stage* ntg_loop_provider_get_stage(ntg_loop_provider* loop_provider);
void ntg_loop_provider_set_stage(ntg_loop_provider* loop_provider,
        ntg_stage* stage);

#endif // _NTG_LOOP_PROVIDER_H_
