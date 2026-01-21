#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include "core/entity/ntg_entity.h"
#include "core/loop/ntg_loop.h"
#include "thirdparty/genc.h"

/* -------------------------------------------------------------------------- */
/* LOOP */
/* -------------------------------------------------------------------------- */

struct ntg_loop
{
    ntg_entity __base;
};

ntg_loop* _ntg_loop_new(ntg_entity_system* system);
void _ntg_loop_init(ntg_loop* loop);
void _ntg_loop_deinit_fn(ntg_entity* entity);

/* -------------------------------------------------------------------------- */
/* PLATFORM */
/* -------------------------------------------------------------------------- */

struct ntg_ptask // Platform task
{
    void (*task_fn)(void* data, ntg_loop_ctx* ctx);
    void* data;
};

GENC_SIMPLE_LIST_GENERATE(ntg_ptask_list, struct ntg_ptask);

struct ntg_platform
{
    ntg_entity __base;

    pthread_mutex_t __lock;
    ntg_ptask_list __tasks;

    // When a loop is forcefully ended, this flag will be set. It indicates that
    // any calls to the platform should be ignored.
    bool __invalid;
};

void _ntg_platform_init(ntg_platform* platform);
void _ntg_platform_deinit(ntg_platform* platform);
void _ntg_platform_execute_later(ntg_platform* platform, struct ntg_ptask task);

void _ntg_platform_execute_all(ntg_platform* platform, ntg_loop_ctx* loop_ctx);
void _ntg_platform_invalidate(ntg_platform* platform);

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
    ntg_entity __base;

    ntg_platform* __platform;

    pthread_t __threads[NTG_LOOP_WORKERS_MAX];
    size_t __thread_count;

    pthread_cond_t __cond;
    pthread_mutex_t __lock;

    ntg_task_list __tasks;

    bool __init;
    size_t __running;

    // When a loop is forcefully ended, this flag will be set. It indicates that
    // any calls to the platform should be ignored.
    bool __invalid;
};

void _ntg_task_runner_init(ntg_task_runner* task_runner,
        ntg_platform* platform, unsigned int worker_threads);

// Make sure not to call deinit when the task runner is running tasks.
// It will attempt to join with all of its threads. If any of the tasks are
// stuck, the main thread will be blocked.
void _ntg_task_runner_deinit(ntg_task_runner* task_runner);

bool _ntg_task_runner_is_running(ntg_task_runner* task_runner);
void _ntg_task_runner_execute(ntg_task_runner* task_runner, struct ntg_task task);

void _ntg_task_runner_invalidate(ntg_task_runner* task_runner);

#endif // _NTG_LOOP_H_
