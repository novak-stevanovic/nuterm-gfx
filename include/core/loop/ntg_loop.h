#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include <stdbool.h>
#include "nt_event.h"

/* -------------------------------------------------------------------------- */
/* DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_loop ntg_loop;

/* Handle to interact with the loop. */
typedef struct ntg_loop_ctx ntg_loop_ctx;

typedef struct ntg_stage ntg_stage;
typedef struct ntg_event_dlgt ntg_event_dlgt;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_taskmaster ntg_taskmaster;
typedef struct ntg_renderer ntg_renderer;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

typedef void (*ntg_loop_process_event_fn)(
        ntg_loop* loop,
        ntg_loop_ctx* ctx,
        struct nt_event event);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* LOOP */
/* ------------------------------------------------------ */

void _ntg_loop_init_(ntg_loop* loop,
        ntg_loop_process_event_fn process_event_fn,
        ntg_stage* init_stage,
        ntg_renderer* renderer,
        ntg_taskmaster* taskmaster,
        unsigned int framerate,
        void* data);
void _ntg_loop_deinit_(ntg_loop* loop);

void ntg_loop_run(ntg_loop* loop, void* ctx_data);

/* Raises NTG_EVT_APP_RESIZE, NTG_EVT_APP_KEY */
ntg_listenable* ntg_loop_get_listenable(ntg_loop* loop);

/* ------------------------------------------------------ */
/* LOOP_CTX */
/* ------------------------------------------------------ */

ntg_stage* ntg_loop_ctx_get_stage(ntg_loop_ctx* ctx);
void ntg_loop_ctx_set_stage(ntg_loop_ctx* ctx, ntg_stage* stage);
void ntg_loop_ctx_break(ntg_loop_ctx* ctx);
struct ntg_xy ntg_loop_ctx_get_app_size(ntg_loop_ctx* ctx);
ntg_taskmaster* ntg_loop_ctx_get_taskmaster(ntg_loop_ctx* ctx);
void* ntg_loop_ctx_get_data(ntg_loop_ctx* ctx);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

/* The library doesn't support full custom-made loops. This is because some
 * types rely on events raised by ntg_loop to work correctly(ntg_db_renderer,
 * for example). Also, many types rely upon ntg_loop_ctx to interact
 * with the loop.
 * 
 * This type represents an abstraction over the loop - `process_event_fn` will
 * be called each time an event occurs. */
struct ntg_loop
{
    ntg_loop_process_event_fn __process_event_fn;
    ntg_stage* __init_stage;
    ntg_renderer* __renderer;
    ntg_taskmaster* __taskmaster;
    unsigned int __framerate;
    void* _data;

    ntg_event_dlgt* __delegate;
};

#endif // _NTG_LOOP_H_
