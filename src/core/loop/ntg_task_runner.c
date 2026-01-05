#include <assert.h>
#include "core/loop/ntg_task_runner.h"
#include "core/entity/ntg_entity.h"
#include "core/entity/ntg_entity_type.h"

ntg_task_runner* ntg_task_runner_new(ntg_entity_system* es)
{
    struct ntg_entity_init_data entity_data = {
        .deinit_fn = _ntg_task_runner_deinit_fn,
        .system = es,
        .type = &NTG_ENTITY_TASK_RUNNER
    };

    ntg_task_runner* new = (ntg_task_runner*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_task_runner_deinit_fn(ntg_entity* entity)
{
    // ntg_task_runner* task_runner = (ntg_task_runner*)entity;
    assert(0);
}

void ntg_task_runner_init(ntg_task_runner* runner, unsigned int thread_pool_size)
{
    assert(runner != NULL);
    assert(thread_pool_size > 0);
    assert(thread_pool_size <= NTG_TASK_RUNNER_MAX_THREADS);
    assert(0);
}

void ntg_task_runner_perform(ntg_task_runner* task_runner,
        void (*task_fn)(void* data, ntg_platform* platform),
        void* data)
{
    assert(0);
}
