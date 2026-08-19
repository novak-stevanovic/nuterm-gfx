#ifndef NTG_TASK_RUNNER_H
#define NTG_TASK_RUNNER_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* TASK RUNNER */
/* ------------------------------------------------------ */

#define NTG_TASK_RUNNER_WORKERS_AUTO 16

int ntg_task_runner_new(unsigned int workers, ntg_task_runner** out_runner);
int ntg_task_runner_destroy(ntg_task_runner* runner);

#define NTG_TASK_RUNNER_PRIORITY_DEFAULT 0

int ntg_task_runner_execute(
        ntg_task_runner* runner,
        void (*task_fn)(void* data, ntg_task_cancel_token* cancel),
        void* data,
        unsigned int priority);

bool ntg_task_runner_is_active(ntg_task_runner* runner);

/* ------------------------------------------------------ */
/* TASK CANCEL TOKEN */
/* ------------------------------------------------------ */

bool ntg_task_cancel_token_stopped(ntg_task_cancel_token* cancel);

#endif // NTG_TASK_RUNNER_H
