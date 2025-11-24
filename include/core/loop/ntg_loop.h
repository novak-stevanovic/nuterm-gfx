#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include <stdbool.h>
#include "nt_event.h"

#include "core/loop/ntg_loop_context.h"

typedef struct ntg_loop ntg_loop;
typedef struct ntg_stage ntg_stage;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef struct ntg_listenable ntg_listenable;

struct ntg_loop_status
{
    // bool break_loop;
    struct nt_event event;
};

typedef ntg_loop_context* (*ntg_loop_create_context_fn)(ntg_loop* loop,
        void* context_data);
typedef void (*ntg_loop_destroy_context_fn)(ntg_loop_context* context);
typedef struct ntg_loop_status (*ntg_loop_fn)(ntg_loop_context* context);

struct ntg_loop
{
    ntg_loop_create_context_fn __create_context_fn;
    ntg_loop_destroy_context_fn __destroy_context_fn;
    ntg_loop_fn __loop_fn;

    ntg_stage* __init_stage;

    ntg_event_delegate* __delegate;

    void* __data;
};

void __ntg_loop_init__(ntg_loop* loop,
        ntg_loop_create_context_fn create_context_fn,
        ntg_loop_destroy_context_fn destroy_context_fn,
        ntg_loop_fn loop_fn,
        ntg_stage* init_stage,
        void* data);
void __ntg_loop_deinit__(ntg_loop* loop);

void ntg_loop_run(ntg_loop* loop, void* context_data);

/* Raises NTG_EVT_APP_RESIZE, NTG_EVT_APP_KEY */
ntg_listenable* ntg_loop_get_listenable(ntg_loop* loop);

#endif // _NTG_LOOP_H_
