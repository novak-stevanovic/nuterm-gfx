#ifndef NTG_LOOP_H
#define NTG_LOOP_H

#include "shared/ntg_shared.h"

#define NTG_LOOP_WORKERS_AUTO 8
#define NTG_LOOP_WORKERS_MAX 32

#define NTG_LOOP_FRAMERATE_AUTO 60
#define NTG_LOOP_FRAMERATE_MAX 500

#define NTG_LOOP_ARENA_SIZE_AUTO ((size_t)2000000)

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* TYPES */
/* -------------------------------------------------------------------------- */

enum ntg_loop_status
{
    NTG_LOOP_DEINIT = 0,
    NTG_LOOP_READY,
    NTG_LOOP_RUNNING,
    NTG_LOOP_STOPPING,
};

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_init(
        ntg_renderer* renderer,
        bool (*dispatch_event_fn)(struct nt_event event),
        unsigned int workers,
        size_t arena_size,
        ntg_stage* init_stage,
        int* out_status);

// What if there are active tasks?
NTG_API void
ntg_loop_deinit(int* out_status);

NTG_API ntg_loop_status
ntg_loop_get_status();

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_loop_dispatch_event_fn_default(struct nt_event event);

/* ------------------------------------------------------ */
/* START */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_start(unsigned int framerate, int* out_status);

NTG_API void
ntg_loop_stop();

/* ------------------------------------------------------ */
/* TASKS & PLATFORM */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_run_task(/* ??? */);

NTG_API bool
ntg_loop_has_running_tasks();

NTG_API void
ntg_loop_execute(/* ??? */);

/* ------------------------------------------------------ */
/* IN-LOOP */
/* ------------------------------------------------------ */

NTG_API ntg_stage*
ntg_loop_get_stage();

NTG_API void
ntg_loop_set_stage(ntg_stage* stage, int* out_status);

NTG_API struct ntg_xy
ntg_loop_get_app_size();

NTG_API unsigned int
ntg_loop_get_framerate();

#endif // NTG_LOOP_H
