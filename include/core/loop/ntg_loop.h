#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include <stdint.h>
#include "shared/ntg_typedef.h"
#include "base/ntg_xy.h"

#define NTG_LOOP_WORKER_THREADS_MAX 32

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_loop_ctx
{
    struct ntg_xy _app_size;
    ntg_stage* _stage;
    uint64_t _elapsed; // elapsed ms since loop started
    uint64_t _frame; // frame counter
    sarena* _arena; // rewinds on frame end, useful for layout
    ntg_task_runner* _task_runner;
    ntg_platform* _platform;
    void* data;

    bool __loop;
};

enum ntg_loop_event_mode
{
    NTG_LOOP_EVENT_PROCESS_FIRST,
    NTG_LOOP_EVENT_DISPATCH_FIRST
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

struct ntg_loop_run_data
{
    ntg_loop_event_fn event_fn;
    ntg_loop_event_mode event_mode;
    ntg_stage* stage;
    ntg_renderer* renderer; // non-NULL
    unsigned int framerate;
    unsigned int worker_threads;

    void* ctx_data;
};

void ntg_loop_run(ntg_loop* loop, struct ntg_loop_run_data data);

void ntg_loop_ctx_break(ntg_loop_ctx* ctx);

void ntg_task_runner_execute(ntg_task_runner* task_runner, 
        void (*task_fn)(void* data, ntg_platform* platform),
        void* data);

void ntg_platform_execute_later(ntg_platform* platform, 
        void (*task_fn)(void* data, ntg_loop_ctx* ctx),
        void* data);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

#endif // _NTG_LOOP_H_
