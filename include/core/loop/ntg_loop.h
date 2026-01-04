#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include <stdint.h>
#include "shared/ntg_typedef.h"
#include "base/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_loop_ctx
{
    bool __loop;
    struct ntg_xy _app_size;
    ntg_stage* _stage;
    uint64_t _elapsed;
    uint64_t _frame;
    sarena* _arena;
    void* data;
};

enum ntg_loop_event_mode
{
    NTG_LOOP_EVENT_PROCESS_FIRST,
    NTG_LOOP_EVENT_DISPATCH_FIRST
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

struct ntg_loop_run_data
{
    ntg_loop_event_fn event_fn;
    ntg_loop_event_mode event_mode;
    ntg_stage* stage;
    ntg_renderer* renderer; // non-NULL
    unsigned int framerate;

    void* ctx_data;
};

void ntg_loop_run(ntg_loop* loop, struct ntg_loop_run_data data);

void ntg_loop_ctx_break(ntg_loop_ctx* ctx);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

#endif // _NTG_LOOP_H_
