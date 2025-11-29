#ifndef _NTG_TASK_MANAGER_H_
#define _NTG_TASK_MANAGER_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct ntg_task_manager ntg_task_manager;
typedef struct ntg_callback_queue_channel ntg_callback_queue_channel;
typedef struct ntg_lock ntg_lock;
typedef struct ntg_task_thread ntg_task_thread;

struct ntg_task_manager
{
    ntg_callback_queue_channel* __channel;
    ntg_lock* __lock;
    ntg_task_thread** __threads;
};

void __ntg_task_manager_init__(
        ntg_task_manager* task_manager,
        ntg_callback_queue_channel* channel);
void __ntg_task_manager_deinit__(ntg_task_manager* task_manager);

/* Executes task on another thread.
 *
 * Appends `callback_fn` to ntg_callback_queue for execution. */
void ntg_task_manager_execute_task(
        ntg_task_manager* task_manager,
        void* (*task_fn)(void* task_data),
        void* task_data,
        void (*callback_fn)(void* callback_data, void* task_result),
        void* callback_data);

/* -------------------------------------------------------------------------- */

#endif // _NTG_TASK_MANAGER_H_
