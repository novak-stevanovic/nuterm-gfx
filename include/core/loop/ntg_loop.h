#ifndef NTG_LOOP_H
#define NTG_LOOP_H

#include <stdint.h>
#include "core/entity/ntg_entity.h"
#include "shared/ntg_xy.h"

#define NTG_LOOP_WORKERS_MAX 32
#define NTG_LOOP_FRAMERATE_MAX 500

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_loop
{
    ntg_entity __base;

    bool _running;
    struct ntg_xy _app_size;
    ntg_stage* _stage;
    unsigned int _framerate;
    uint64_t _elapsed; // elapsed ms since loop started
    uint64_t _frame; // frame counter
    sarena* _arena; // rewinds on frame end, useful for layout

    ntg_renderer* _renderer;
    ntg_task_runner* _task_runner;
    ntg_platform* _platform;

    void* data;

    bool __pending_stage_flag;
    ntg_stage* __pending_stage;
    bool (*on_event_fn)(ntg_loop* loop, struct nt_event event);
    bool __loop, __force_break;
};

enum ntg_loop_end_mode
{
    NTG_LOOP_END_CLEAN,
    NTG_LOOP_END_FORCE,
};

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_loop* ntg_loop_new(ntg_entity_system* system);
void ntg_loop_init(ntg_loop* loop, ntg_stage* init_stage, ntg_renderer* renderer,
                   unsigned int framerate, unsigned int workers);
void ntg_loop_deinit(ntg_loop* loop);

enum ntg_loop_end_mode ntg_loop_run(ntg_loop* loop);

/* Used to stop the main loop. When this function is called, an issue may occur
 * if the task runner is still running tasks on worker threads. In this case,
 * behavior of the function depends on `end_mode` parameter.
 * 
 * If `end_mode` == NTG_LOOP_END_CLEAN, the function will fail and will return
 * false.
 *
 * If `end_mode` == NTG_LOOP_END_FORCE, the function will succeed regardless and
 * will return true. `ntg_loop_run()` will return NTG_LOOP_END_FORCE.
 *
 * The user should then clean up and terminate the program. This will leave the
 * threads running. If any of the tasks use `ntg_platform_execute_later`, the
 * request will be ignored. */
bool ntg_loop_break(ntg_loop* loop, ntg_loop_end_mode end_mode);
void ntg_loop_set_stage(ntg_loop* loop, ntg_stage* stage);

void ntg_task_runner_execute(ntg_task_runner* task_runner, 
                             void (*task_fn)(void* data, ntg_platform* platform),
                             void* data);

void ntg_platform_execute_later(ntg_platform* platform, 
                                void (*task_fn)(void* data),
                                void* data);

// Default implementation of `on_event_fn` - dispatches key/mouse event to stage
bool ntg_loop_dispatch_event(ntg_loop* loop, struct nt_event event);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

#endif // NTG_LOOP_H
