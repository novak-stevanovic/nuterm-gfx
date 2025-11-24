#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include <stdbool.h>
#include "nt_event.h"

typedef struct ntg_loop ntg_loop;
typedef struct ntg_stage ntg_stage;
typedef struct ntg_loop_context ntg_loop_context;

// TODO: Introduce ntg_loop_context

struct ntg_loop_status
{
    // bool break_loop;
    struct nt_event event;
};

/* This function represents the inner part of loop. */
typedef struct ntg_loop_status (*ntg_loop_fn)(ntg_loop_context* context);

struct ntg_loop
{
    ntg_loop_fn __run_fn;
    ntg_stage* __init_stage;
};

void __ntg_loop_init__(ntg_loop* loop,
        ntg_loop_fn run_fn,
        ntg_stage* init_stage);
void __ntg_loop_deinit__(ntg_loop* loop);

void ntg_loop_run(ntg_loop* loop);

#endif // _NTG_LOOP_H_
