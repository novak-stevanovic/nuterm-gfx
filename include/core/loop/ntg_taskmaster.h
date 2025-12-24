#ifndef _NTG_TASKMASTER_H_
#define _NTG_TASKMASTER_H_

#include <stdbool.h>
#include <stddef.h>
#include "base/entity/ntg_entity.h"

typedef struct ntg_taskmaster ntg_taskmaster;
typedef struct ntg_callback_queue ntg_callback_queue;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_taskmaster
{
    ntg_entity __base;

    ntg_callback_queue* __callbacks;
    void* data;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_taskmaster* ntg_taskmaster_new(ntg_entity_system* system);
void _ntg_taskmaster_init_(ntg_taskmaster* taskmaster);

void ntg_taskmaster_execute_callbacks(ntg_taskmaster* taskmaster);

/* Tells task manager to execute task on a different thread.
 *
 * Appends `callback_fn` to ntg_platform for execution on the main thread.
 *
 * `task_fn` should free resources tied to `task_data`.
 * `callback_fn` should free resources tied to `callback_data` and `task_result` */
void ntg_taskmaster_execute_task(
        ntg_taskmaster* taskmaster,
        void* (*task_fn)(void* task_data),
        void* task_data,
        void (*callback_fn)(void* callback_data, void* task_result),
        void* callback_data);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_taskmaster_deinit_fn(ntg_entity* entity);

#endif // _NTG_TASKMASTER_H_
