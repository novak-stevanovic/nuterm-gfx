#include <assert.h>
#include <stdlib.h>

#include "core/platform/ntg_task_manager.h"
#include "shared/ntg_lock.h"

void __ntg_task_manager_init__(
        ntg_task_manager* task_manager,
        ntg_task_manager_execute_fn execute_fn,
        ntg_platform_channel* platform_channel)
{
    assert(task_manager != NULL);

    task_manager->_platform_channel = platform_channel;
    task_manager->__execute_fn = execute_fn;
    task_manager->_lock = ntg_lock_new();
}

void __ntg_task_manager_deinit__(ntg_task_manager* task_manager)
{
    assert(task_manager != NULL);

    task_manager->_platform_channel = NULL;
    task_manager->__execute_fn = NULL;
    ntg_lock_destroy(task_manager->_lock);
    task_manager->_lock = NULL;
}

void ntg_task_manager_execute_task(
        ntg_task_manager* task_manager,
        bool (*task_fn)(void* task_data),
        void* task_data,
        size_t task_data_size,
        void (*callback_fn)(void* callback_data),
        void* callback_data,
        size_t callback_data_size)
{
    assert(task_manager != NULL);
    assert(task_fn != NULL);

    ntg_lock_lock(task_manager->_lock);

    task_manager->__execute_fn(task_manager,
            task_fn, task_data, task_data_size,
            callback_fn, callback_data, callback_data_size);

    ntg_lock_unlock(task_manager->_lock);
}
