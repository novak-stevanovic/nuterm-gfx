#ifndef NTG_LOOP_H
#define NTG_LOOP_H

#include <stdint.h>
#include "shared/ntg_typedef.h"
#include "shared/ntg_xy.h"

#define NTG_LOOP_WORKERS_MAX 32

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

    bool __loop, __force_break;
};

enum ntg_loop_status
{
    NTG_LOOP_CLEAN_FINISH,
    NTG_LOOP_FORCE_FINISH
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

struct ntg_loop_run_data
{
    ntg_loop_event_fn event_fn;
    ntg_stage* stage;
    ntg_renderer* renderer; // non-NULL
    unsigned int framerate;
    unsigned int workers;

    void* ctx_data;
};

enum ntg_loop_status ntg_loop_run(ntg_loop* loop, struct ntg_loop_run_data data);

/* Used to stop the main loop. When this function is called, an issue may occur
 * if the task runner is still running tasks on worker threads. In this case,
 * behavior of the function depends on `force_break` parameter.
 * 
 * If `force_break` is false, the function will fail and will return `false`.
 * If `force_break` is true, the function will not deinitalize the task runner
 * or the platform. This will signal `ntg_loop_return` to return
 * NTG_LOOP_FORCE_FINISH. The user should then clean up and terminate the program.
 * 
 * This will leave the threads running. If any of the tasks use
 * `ntg_platform_execute_later`, the request will be ignored. */
bool ntg_loop_ctx_break(ntg_loop_ctx* ctx, bool force_break);

void ntg_task_runner_execute(ntg_task_runner* task_runner, 
        void (*task_fn)(void* data, ntg_platform* platform),
        void* data);

void ntg_platform_execute_later(ntg_platform* platform, 
        void (*task_fn)(void* data, ntg_loop_ctx* ctx),
        void* data);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

#endif // NTG_LOOP_H
