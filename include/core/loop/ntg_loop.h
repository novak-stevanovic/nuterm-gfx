#ifndef NTG_LOOP_H
#define NTG_LOOP_H

#include "shared/ntg_shared.h"

/* ========================================================================== */
/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */
/* ========================================================================== */

/* ========================================================================== */
/* TYPES */
/* ========================================================================== */

enum ntg_loop_state
{
    NTG_LOOP_DEINIT = 0,
    NTG_LOOP_READY,
    NTG_LOOP_RUNNING,
    NTG_LOOP_STOPPING,
};

/* ------------------------------------------------------ */

#define NTG_LOOP_WORKERS_AUTO 8
#define NTG_LOOP_ARENA_SIZE_AUTO ((size_t)2000000)

struct ntg_loop_init_opts
{
    size_t arena_size;
};

/* ------------------------------------------------------ */

#define NTG_LOOP_FRAMERATE_AUTO 60

enum ntg_loop_mouse_mode
{
    NTG_LOOP_MOUSE_DISABLE = 0,
    NTG_LOOP_MOUSE_ENABLE
};

struct ntg_loop_start_opts
{
    enum ntg_loop_mouse_mode mouse_mode; 
    unsigned int framerate;
};

/* ========================================================================== */
/* FUNCTIONS */
/* ========================================================================== */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API int
ntg_loop_init(
        ntg_renderer* custom_renderer,
        bool (*on_event_fn)(const struct nt_event* event),
        ntg_stage* init_stage,
        const struct ntg_loop_init_opts* opts);

// What if there are active tasks?
NTG_API int
ntg_loop_deinit(void);

NTG_API enum ntg_loop_state
ntg_loop_get_state(void);

NTG_API bool
ntg_loop_is_running(void);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

NTG_API bool
ntg_loop_dispatch_event_fn_default(const struct nt_event* event);

/* ------------------------------------------------------ */
/* START/STOP */
/* ------------------------------------------------------ */

NTG_API int
ntg_loop_start(const struct ntg_loop_start_opts* opts);

NTG_API void
ntg_loop_stop(void);

/* ------------------------------------------------------ */
/* EXECUTE */
/* ------------------------------------------------------ */

#define NTG_LOOP_DELAY_MS_MAX 86400000L

/* If loop is NTG_LOOP_READY the tasks will be executed on next ntg_loop_start(). */

NTG_API int
ntg_loop_schedule(
        void (*task_fn)(void* data),
        void* data,
        unsigned long delay_ms);

/* May be used before starting the loop or stopping the loop to discard any tasks
 * that were pushed while the loop was inactive. */

NTG_API void
ntg_loop_tasks_clear(void);

NTG_API bool
ntg_loop_has_tasks(void);

/* ------------------------------------------------------ */
/* IN-LOOP ONLY */
/* ------------------------------------------------------ */

NTG_API ntg_stage*
ntg_loop_get_stage(void);

NTG_API int
ntg_loop_set_stage(ntg_stage* stage);

NTG_API struct ntg_xy
ntg_loop_get_app_size(void);

NTG_API unsigned int
ntg_loop_get_framerate(void);

#endif // NTG_LOOP_H
