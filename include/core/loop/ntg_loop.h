#ifndef NTG_LOOP_H
#define NTG_LOOP_H

#include "shared/ntg_shared.h"

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

/* ------------------------------------------------------ */

#define NTG_LOOP_WORKERS_AUTO 8
#define NTG_LOOP_ARENA_SIZE_AUTO ((size_t)2000000)

struct ntg_loop_init_opts
{
    size_t arena_size;
};

NTG_API struct ntg_loop_init_opts
ntg_loop_init_opts_default();

/* ------------------------------------------------------ */

#define NTG_LOOP_FRAMERATE_AUTO 60
#define NTG_LOOP_FRAMERATE_MAX 500

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

NTG_API struct ntg_loop_start_opts
ntg_loop_start_opts_default();

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* INIT/DEINIT */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_init(
        ntg_renderer* custom_renderer,
        bool (*dispatch_event_fn)(const struct nt_event* event),
        ntg_stage* init_stage,
        const struct ntg_loop_init_opts* opts,
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
ntg_loop_dispatch_event_fn_default(const struct nt_event* event);

/* ------------------------------------------------------ */
/* START */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_start(const struct ntg_loop_start_opts* opts, int* out_status);

NTG_API void
ntg_loop_stop();

/* ------------------------------------------------------ */
/* IN-LOOP */
/* ------------------------------------------------------ */

NTG_API void
ntg_loop_execute(/* ??? */);

NTG_API ntg_stage*
ntg_loop_get_stage();

NTG_API void
ntg_loop_set_stage(ntg_stage* stage, int* out_status);

NTG_API struct ntg_xy
ntg_loop_get_app_size();

NTG_API unsigned int
ntg_loop_get_framerate();

#endif // NTG_LOOP_H
