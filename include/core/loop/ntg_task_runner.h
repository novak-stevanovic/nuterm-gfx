#ifndef _NTG_TASK_RUNNER_H_
#define _NTG_TASK_RUNNER_H_

#include "shared/ntg_typedef.h"

#define NTG_TASK_RUNNER_MAX_THREADS 32

ntg_task_runner* ntg_task_runner_new(ntg_entity_system* es);
void _ntg_task_runner_deinit_fn(ntg_entity* entity);
void ntg_task_runner_init(ntg_task_runner* runner, unsigned int thread_pool_size);

void ntg_task_runner_perform(ntg_task_runner* task_runner,
        void (*task_fn)(void* data, ntg_platform* platform),
        void* data);

#endif // _NTG_TASK_RUNNER_H_
