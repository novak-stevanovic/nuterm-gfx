#ifndef _NTG_DEF_TASK_MANAGER_H_
#define _NTG_DEF_TASK_MANAGER_H_

#include "core/platform/ntg_task_manager.h"

typedef struct ntg_def_task_manager ntg_def_task_manager;
typedef struct sarena sarena;

struct ntg_def_task_manager
{
    ntg_task_manager __base;

    sarena* __arena;
    
    unsigned int __active_tasks;
};

void __ntg_def_task_manager_init__(
        ntg_def_task_manager* task_manager,
        ntg_platform_channel* platform_channel);
void __ntg_def_task_manager_deinit__(ntg_def_task_manager* task_manager);

void __ntg_def_task_manager_execute_fn(
        ntg_task_manager* manager,
        bool (*task_fn)(void* task_data),
        void* task_data,
        size_t task_data_size,
        void (*callback_fn)(void* callback_data),
        void* callback_data,
        size_t callback_data_size);

#endif // _NTG_DEF_TASK_MANAGER_H_
