#ifndef _NTG_TASKMASTER_H_
#define _NTG_TASKMASTER_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct ntg_taskmaster ntg_taskmaster;
typedef struct ntg_taskmaster_channel ntg_taskmaster_channel;

typedef struct ntg_callback_queue ntg_callback_queue;

ntg_taskmaster* ntg_taskmaster_new();
void ntg_taskmaster_destroy(ntg_taskmaster* taskmaster);

void ntg_taskmaster_execute_callbacks(ntg_taskmaster* taskmaster);

ntg_taskmaster_channel* ntg_taskmaster_get_channel(ntg_taskmaster* taskmaster);

/* Tells task manager to execute task on a different thread.
 *
 * Appends `callback_fn` to ntg_platform for execution on the main thread.
 *
 * `task_fn` should free resources tied to `task_data`.
 * `callback_fn` should free resources tied to `callback_data` and `task_result` */
void ntg_taskmaster_execute_task(
        ntg_taskmaster_channel* _taskmaster,
        void* (*task_fn)(void* task_data),
        void* task_data,
        void (*callback_fn)(void* callback_data, void* task_result),
        void* callback_data);

/* -------------------------------------------------------------------------- */
/* PRIVATE */
/* -------------------------------------------------------------------------- */

#ifdef _NTG_TASKMASTER_PRIVATE_

#include <pthread.h>

struct ntg_taskmaster_channel
{
    ntg_taskmaster* __taskmaster;
};

struct ntg_taskmaster
{
    ntg_callback_queue* __callbacks;
    ntg_taskmaster_channel __channel;
    pthread_mutex_t __lock;
};

void __ntg_taskmaster_init__(ntg_taskmaster* taskmaster);
void __ntg_taskmaster_deinit__(ntg_taskmaster* taskmaster);

#endif // _NTG_TASKMASTER_PRIVATE_

#endif // _NTG_TASKMASTER_H_
