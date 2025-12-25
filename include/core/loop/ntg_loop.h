#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include <stdbool.h>
#include "nt_event.h"
#include "shared/ntg_xy.h"

typedef struct ntg_loop ntg_loop;

/* Handle to interact with the loop. */
typedef struct ntg_loop_ctx ntg_loop_ctx;

typedef struct ntg_stage ntg_stage;
typedef struct ntg_taskmaster ntg_taskmaster;
typedef struct ntg_renderer ntg_renderer;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_loop_ctx
{
    bool __loop;
    struct ntg_xy _app_size;
    ntg_taskmaster* _taskmaster;
    ntg_stage* _stage;
    void* data;
    unsigned int _elapsed;
};

struct ntg_loop_event
{
    struct nt_event event;
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

struct ntg_loop_run_data
{
    void (*process_event_fn)(ntg_loop_ctx* ctx, struct nt_event event);
    ntg_stage* stage;
    ntg_renderer* renderer; // non-NULL
    ntg_taskmaster* taskmaster; // non-NULL
    unsigned int framerate;

    void* ctx_data;
};

void ntg_loop_run(ntg_loop* loop, struct ntg_loop_run_data data);

void ntg_loop_ctx_break(ntg_loop_ctx* ctx);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

#endif // _NTG_LOOP_H_
