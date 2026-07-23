#ifndef NTG_LOOP_H
#define NTG_LOOP_H

#include <stdint.h>
#include "shared/ntg_shared.h"
#include "base/ntg_xy.h"

#define NTG_LOOP_WORKERS_MAX 32
#define NTG_LOOP_FRAMERATE_MAX 500

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

enum ntg_loop_status
{
    NTG_LOOP_READY,
    NTG_LOOP_RUNNING,
    NTG_LOOP_STOPPING,
    NTG_LOOP_END
};

enum ntg_loop_stop_mode
{
    NTG_LOOP_STOP_CLEAN,
    NTG_LOOP_STOP_FORCE
};

enum ntg_loop_exit_status
{
    NTG_LOOP_EXIT_CLEAN,
    NTG_LOOP_EXIT_PREMATURE,
    NTG_LOOP_EXIT_ERROR
};

struct ntg_loop
{
    enum ntg_loop_status _status;

    struct ntg_xy _app_size;
    ntg_loop_exit_status __exit_status;
    ntg_stage* _stage;
    unsigned int _framerate;
    uint64_t _elapsed; // elapsed ms since loop started
    uint64_t _frame; // frame counter

    // initialized when the loop starts, rewinds on frame end, used for layout
    sarena* _arena;

    // if NULL in init(), creates ntg_def_renderer on start
    ntg_renderer* _renderer;

    ntg_task_runner* _task_runner;
    ntg_platform* _platform;

    void* data;

    ntg_stage* __pending_stage;
    bool (*__on_event_fn)(ntg_loop* loop, struct nt_event event);
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

/* Default loop event handler. It converts supported terminal key and mouse
 * events and forwards them to the current stage.
 *
 * RETURN VALUE:
 * `true` when the stage handles the event; otherwise `false`. */
NTG_API bool
ntg_loop_dispatch_event(ntg_loop* loop, struct nt_event event);

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

/* Initializes a loop with its initial stage, renderer, target frame rate,
 * worker count, and event callback. A `NULL` renderer selects the default
 * renderer when the loop starts. Values above the frame-rate and worker limits
 * are clamped; a `NULL` event callback disables event dispatch.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `loop` or `init_stage` is `NULL`, or `framerate` is
 *   zero.
 * - `NTG_ERR_STAGE_HAS_LOOP`: the initial stage already belongs to a loop.
 * - `NTG_ERR_ALLOC_FAIL`: required loop-owned objects cannot be allocated.
 * - `NTG_ERR_UNEXPECTED`: platform, worker, or stage initialization fails. */
NTG_API void
ntg_loop_init(ntg_loop* loop,
        ntg_stage* init_stage,
        ntg_renderer* renderer,
        unsigned int framerate,
        unsigned int workers,
        bool (*on_event_fn)(ntg_loop* loop, struct nt_event event),
        int* out_status);

/* ------------------------------------------------------ */

/* Disconnects the stage and resets the loop. After a clean exit it also joins
 * workers and frees the platform and task runner; after a premature exit those
 * resources are intentionally left allocated and invalidated.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `loop` is `NULL`.
 * - `NTG_ERR_LOOP_RUNNING`: the loop is currently in `NTG_LOOP_RUNNING`. */
NTG_API void
ntg_loop_deinit(ntg_loop* loop, int* out_status);

/* ------------------------------------------------------ */

/* Void-pointer adapter for `ntg_loop_deinit`, intended for cleanup callbacks.
 * Any deinitialization status is discarded. */
NTG_API void
ntg_loop_deinit_v(void* _loop);

/* ------------------------------------------------------ */
/* CONTROL */
/* ------------------------------------------------------ */

/* Runs terminal event processing, deferred tasks, layout, composition, and
 * rendering until a stop is requested. The function rejects only a loop that is
 * already running.
 *
 * RETURN VALUE:
 * The final loop exit status. Immediate validation or setup failure returns
 * `NTG_LOOP_EXIT_ERROR`.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `loop` is `NULL`.
 * - `NTG_ERR_LOOP_RUNNING`: the loop is already in `NTG_LOOP_RUNNING`.
 * - `NTG_ERR_ALLOC_FAIL`: the frame arena or default renderer cannot be
 *   allocated.
 * - `NTG_ERR_UNEXPECTED`: default-renderer initialization fails
 * unexpectedly. */
NTG_API enum ntg_loop_exit_status
ntg_loop_run(ntg_loop* loop, int* out_status);

/* ------------------------------------------------------ */

/* Requests loop termination. Force mode always accepts the request and marks
 * the exit premature only when worker tasks are running; clean mode refuses
 * while tasks are running.
 *
 * RETURN VALUE:
 * `true` for a `NULL` loop, every force request, or a clean request with no
 * running tasks; otherwise `false`. */
NTG_API bool
ntg_loop_break(ntg_loop* loop, ntg_loop_stop_mode stop_mode);

/* ------------------------------------------------------ */

/* Sets the loop stage immediately while stopped, or records it as pending while
 * running or stopping. A stage owned by another loop is first detached from
 * that loop; `NULL` clears the stage.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `loop` is `NULL`. */
NTG_API void
ntg_loop_set_stage(ntg_loop* loop, ntg_stage* stage, int* out_status);

/* ------------------------------------------------------ */

/* Declares a query for whether a loop is active. No implementation for this
 * declaration exists in the current source tree, so its runtime behavior cannot
 * be verified. */
NTG_API bool
ntg_loop_is_running(const ntg_loop* loop);

/* ------------------------------------------------------ */
/* TASKS */
/* ------------------------------------------------------ */

/* Queues a callback for execution on a worker thread. If the runner has been
 * invalidated, the request is silently ignored.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `task_runner` or `task_fn` is `NULL`.
 * - `NTG_ERR_ALLOC_FAIL`: the task cannot be queued.
 * - `NTG_ERR_UNEXPECTED`: the worker queue operation fails unexpectedly. */
NTG_API void
ntg_task_runner_execute(
        ntg_task_runner* task_runner,
        void (*task_fn)(void* data, ntg_platform* platform),
        void* data,
        int* out_status);

/* ------------------------------------------------------ */

/* Queues a function to run on the loop thread during the next platform-task
 * drain. This is the safe path for worker tasks to request UI-thread work.
 *
 * ERROR CODES:
 * - `NTG_ERR_INVALID_ARG`: `platform` or `task_fn` is `NULL`.
 * - `NTG_ERR_PLATFORM_NO_LOOP`: the platform has been invalidated because its
 *   loop stopped forcefully.
 * - `NTG_ERR_ALLOC_FAIL`: the deferred task cannot be queued.
 * - `NTG_ERR_UNEXPECTED`: the platform queue operation fails unexpectedly. */
NTG_API void
ntg_platform_execute_later(
        ntg_platform* platform,
        void (*task_fn)(void* data),
        void* data,
        int* out_status);

#endif // NTG_LOOP_H
