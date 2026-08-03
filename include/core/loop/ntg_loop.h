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
    uint64_t _elapsed; 
    uint64_t _frame; 

    sarena* _arena;

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

NTG_API bool
ntg_loop_dispatch_event(ntg_loop* loop, struct nt_event event);

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_init(
        ntg_loop* loop,
        ntg_stage* init_stage,
        ntg_renderer* renderer,
        unsigned int framerate,
        unsigned int workers,
        bool (*handle_event_fn)(ntg_loop* loop, struct nt_event event),
        int* out_status);

/* ------------------------------------------------------ */

NTG_API void
ntg_loop_deinit(ntg_loop* loop, int* out_status);

/* ------------------------------------------------------ */

NTG_API void
ntg_loop_deinit_void(void* _loop);

/* ------------------------------------------------------ */
/* CONTROL */
/* ------------------------------------------------------ */

NTG_API enum ntg_loop_exit_status
ntg_loop_run(ntg_loop* loop, int* out_status);

/* ------------------------------------------------------ */

NTG_API bool
ntg_loop_break(ntg_loop* loop, ntg_loop_stop_mode stop_mode);

/* ------------------------------------------------------ */

NTG_API void
ntg_loop_set_stage(ntg_loop* loop, ntg_stage* stage, int* out_status);

/* ------------------------------------------------------ */

NTG_API bool
ntg_loop_is_running(const ntg_loop* loop);

/* ------------------------------------------------------ */
/* TASKS */
/* ------------------------------------------------------ */

NTG_API void
ntg_task_runner_execute(
        ntg_task_runner* task_runner,
        void (*task_fn)(void* data, ntg_platform* platform),
        void* data,
        int* out_status);

/* ------------------------------------------------------ */

NTG_API void
ntg_platform_execute_later(
        ntg_platform* platform,
        void (*task_fn)(void* data),
        void* data,
        int* out_status);

#endif // NTG_LOOP_H
