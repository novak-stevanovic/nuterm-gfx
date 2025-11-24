#ifndef _NTG_DEF_LOOP_H_
#define _NTG_DEF_LOOP_H_

#include <stdbool.h>
#include "core/loop/ntg_loop.h"

typedef struct ntg_def_loop ntg_def_loop;
typedef struct ntg_renderer ntg_renderer;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_delegate ntg_event_delegate;
struct nt_key_event;

/* Returns if the key had been consumed */
typedef bool (*ntg_def_loop_process_key_fn)(
        ntg_def_loop* loop,
        struct nt_key_event key,
        void* data);

struct ntg_def_loop
{
    ntg_loop __base;

    unsigned int __framerate;
    ntg_renderer* __renderer;
    ntg_def_loop_process_key_fn __process_key_fn;
    void* __process_key_data;

    ntg_event_delegate* __delegate;
};

void __ntg_def_loop_init__(
        ntg_def_loop* loop,
        ntg_stage* init_stage,
        unsigned int framerate,
        ntg_renderer* renderer,
        ntg_def_loop_process_key_fn process_key_fn,
        void* process_key_fn_data);
void __ntg_def_loop_deinit__(ntg_def_loop* loop);

/* Raises NTG_EVT_APP_RESIZE */
ntg_listenable* ntg_def_loop_get_listenable(
        ntg_def_loop* loop);

void __ntg_def_loop_run_fn(ntg_loop* _loop);

#endif // _NTG_DEF_LOOP_H_
