#ifndef NTG_LOOP_INTERNAL_H
#define NTG_LOOP_INTERNAL_H

#include "shared/ntg_shared.h"
#include "core/loop/ntg_loop.h"
#include "thirdparty/genc.h"

/* ========================================================================== */
/* INTERNAL - TYPES AND FUNCTIONS */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* PLATFORM */
/* -------------------------------------------------------------------------- */

struct ntg_ptask // Platform task
{
    void (*task_fn)(void* data);
    void* data;
};

GENC_SIMPLE_LIST_GENERATE(ntg_ptask_list, struct ntg_ptask);

struct ntg_platform
{
    // When a loop is forcefully ended, loop will be NULL. It indicates that
    // any calls to the platform should be ignored.
    ntg_loop* __loop;

    pthread_mutex_t __lock;
    ntg_ptask_list __tasks;
};

/* Initializes the internal deferred-task platform and associates it with
 * `loop`.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `platform` or `loop` is `NULL`. */
void _ntg_platform_init(ntg_platform* platform, ntg_loop* loop, int* out_status);
/* Releases the platform task queue and synchronization objects. Passing `NULL`
 * has no effect. */
void _ntg_platform_deinit(ntg_platform* platform);
/* Queues an already constructed platform task for later execution on the loop
 * thread. The callback is not validated and must be usable when the queue is
 * drained.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `platform` is `NULL`.
 * - `NTG_ERR_PLATFORM_NO_LOOP`: the platform is invalidated and no longer has a
 *   loop.
 * - `NTG_ERR_ALLOC_FAIL`: the task cannot be queued.
 * - `NTG_ERR_UNEXPECTED`: the queue operation fails unexpectedly. */
void _ntg_platform_execute_later(
        ntg_platform* platform,
        struct ntg_ptask task,
        int* out_status);

/* Executes and removes every deferred platform task currently queued. A `NULL`
 * or invalid platform is ignored. */
void _ntg_platform_execute_all(ntg_platform* platform);
/* Detaches the platform from its loop so future deferred requests are rejected.
 * Passing `NULL` has no effect. */
void _ntg_platform_invalidate(ntg_platform* platform);
/* Checks whether the platform is still associated with a loop. */
bool _ntg_platform_is_valid(ntg_platform* platform);

/* -------------------------------------------------------------------------- */
/* TASK RUNNER */
/* -------------------------------------------------------------------------- */

#define NTG_TASK_RUNNER_TASKS_MAX 1000

struct ntg_task
{
    void (*task_fn)(void* data, ntg_platform* platform);
    void* data;
};

GENC_SIMPLE_LIST_GENERATE(ntg_task_list, struct ntg_task);

struct ntg_task_runner
{
    ntg_platform* __platform;

    // When a loop is forcefully ended, loop will be NULL. It indicates that
    // any calls to the task_runner should be ignored.
    ntg_loop* __loop;

    pthread_t __threads[NTG_LOOP_WORKERS_MAX];
    size_t __thread_count;

    pthread_cond_t __cond;
    pthread_mutex_t __lock;

    ntg_task_list __tasks;

    bool __init;
    size_t __running;
};

/* Initializes the worker queue and creates `worker_threads` threads. The `loop`
 * pointer is stored but not validated.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `task_runner` or `platform` is `NULL`, or
 *   `worker_threads` exceeds `NTG_LOOP_WORKERS_MAX`.
 * - `NTG_ERR_THREAD_SPAWN`: a worker thread cannot be created. */
void _ntg_task_runner_init(
        ntg_task_runner* task_runner,
        ntg_platform* platform,
        unsigned int worker_threads,
        ntg_loop* loop,
        int* out_status);

/* Stops the worker threads, joins them, and releases queue and synchronization
 * resources. It can block until running tasks finish. */
void _ntg_task_runner_deinit(ntg_task_runner* task_runner);

/* Checks whether at least one worker task is currently executing; queued but
 * not yet running tasks are not counted. */
bool _ntg_task_runner_is_running(ntg_task_runner* task_runner);
/* Queues an already constructed worker task and signals the worker threads. If
 * the runner is stopped or invalidated, the request is silently ignored.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `task_runner` or the task callback is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: the task cannot be queued.
 * - `NTG_ERR_UNEXPECTED`: the queue operation fails unexpectedly. */
void _ntg_task_runner_execute(
        ntg_task_runner* task_runner,
        struct ntg_task task,
        int* out_status);

/* Detaches the task runner from its loop so further task submissions are
 * ignored or rejected. Passing `NULL` has no effect. */
void _ntg_task_runner_invalidate(ntg_task_runner* task_runner);
/* Checks whether the task runner is still associated with a loop. */
bool _ntg_task_runner_is_valid(ntg_task_runner* task_runner);

#endif // NTG_LOOP_INTERNAL_H
