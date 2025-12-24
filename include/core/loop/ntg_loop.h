#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include <stdbool.h>
#include "base/entity/ntg_entity.h"
#include "nt_event.h"

typedef struct ntg_loop ntg_loop;
/* Handle to interact with the loop. */
typedef struct ntg_loop_ctx ntg_loop_ctx;

typedef struct ntg_stage ntg_stage;
typedef struct ntg_taskmaster ntg_taskmaster;
typedef struct ntg_renderer ntg_renderer;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

typedef void (*ntg_loop_process_event_fn)(
        ntg_loop* loop,
        ntg_loop_ctx* ctx,
        struct nt_event event);

/* The library doesn't support full custom-made loops. This is because some
 * types rely on events raised by ntg_loop to work correctly(ntg_db_renderer,
 * for example). Also, many types rely upon ntg_loop_ctx to interact
 * with the loop.
 * 
 * This type represents an abstraction over the loop - `process_event_fn` will
 * be called each time an event occurs. */
struct ntg_loop
{
    ntg_entity __base;

    ntg_loop_process_event_fn __process_event_fn;
    ntg_renderer* _renderer;
    ntg_taskmaster* _taskmaster;
    unsigned int _framerate;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* LOOP */
/* ------------------------------------------------------ */

ntg_loop* ntg_loop_new(ntg_entity_system* system);

struct ntg_loop_init_data
{
    ntg_loop_process_event_fn process_event_fn;
    ntg_renderer* renderer; // non-NULL
    ntg_taskmaster* taskmaster; // non-NULL
    unsigned int framerate;
};

void _ntg_loop_init_(ntg_loop* loop, struct ntg_loop_init_data init_data);

void ntg_loop_run(ntg_loop* loop, ntg_stage* init_stage, void* ctx_data);

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

void _ntg_loop_deinit_fn(ntg_entity* entity);

#endif // _NTG_LOOP_H_
