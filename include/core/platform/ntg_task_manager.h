#ifndef _NTG_TASK_MANAGER_H_
#define _NTG_TASK_MANAGER_H_

#include <stdbool.h>
#include <stddef.h>

typedef struct ntg_task_manager ntg_task_manager;
typedef struct ntg_platform_channel ntg_platform_channel;
typedef struct ntg_lock ntg_lock;

typedef void (*ntg_task_manager_execute_fn)(
        ntg_task_manager* manager,
        bool (*task_fn)(void* task_data),
        void* task_data,
        size_t task_data_size,
        void (*callback_fn)(void* callback_data),
        void* callback_data,
        size_t callback_data_size);

struct ntg_task_manager
{
    ntg_task_manager_execute_fn __execute_fn;
    ntg_platform_channel* _platform_channel;
    ntg_lock* _lock;
};

void __ntg_task_manager_init__(
        ntg_task_manager* task_manager,
        ntg_task_manager_execute_fn execute_fn,
        ntg_platform_channel* platform_channel);
void __ntg_task_manager_deinit__(ntg_task_manager* task_manager);

/* Executes task on another thread.
 *
 * Appends `callback_fn` to platform for execution. */
void ntg_task_manager_execute_task(
        ntg_task_manager* task_manager,
        bool (*task_fn)(void* task_data),
        void* task_data,
        size_t task_data_size,
        void (*callback_fn)(void* callback_data),
        void* callback_data,
        size_t callback_data_size);

#endif // _NTG_TASK_MANAGER_H_
