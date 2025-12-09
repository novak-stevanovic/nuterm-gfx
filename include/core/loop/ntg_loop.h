#ifndef _NTG_LOOP_H_
#define _NTG_LOOP_H_

#include <stdbool.h>
#include "nt_event.h"

typedef struct ntg_loop ntg_loop;
typedef struct ntg_stage ntg_stage;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_taskmaster ntg_taskmaster;
typedef struct ntg_taskmaster_channel ntg_taskmaster_channel;

struct ntg_loop_status
{
    unsigned int timeout;
};

/* Handle to interact with the loop without direct reference to ntg_loop */
typedef struct ntg_loop_ctx ntg_loop_ctx;

ntg_stage* ntg_loop_ctx_get_stage(ntg_loop_ctx* ctx);
void ntg_loop_ctx_set_stage(ntg_loop_ctx* ctx, ntg_stage* stage);
void ntg_loop_ctx_break(ntg_loop_ctx* ctx);
struct ntg_xy ntg_loop_ctx_get_app_size(ntg_loop_ctx* ctx);
ntg_taskmaster_channel* ntg_loop_ctx_get_taskmaster(ntg_loop_ctx* ctx);
void* ntg_loop_ctx_get_data(ntg_loop_ctx* ctx);

typedef struct ntg_loop_status (*ntg_loop_process_event_fn)(
        ntg_loop* loop,
        ntg_loop_ctx* ctx,
        struct nt_event event);

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
    ntg_taskmaster* __taskmaster;
    void* _data;

    ntg_event_delegate* __delegate;
};

void __ntg_loop_init__(ntg_loop* loop,
        ntg_loop_process_event_fn process_event_fn,
        ntg_stage* init_stage,
        ntg_taskmaster* taskmaster,
        void* data);
void __ntg_loop_deinit__(ntg_loop* loop);

void ntg_loop_run(ntg_loop* loop, void* ctx_data);

/* Raises NTG_EVT_APP_RESIZE, NTG_EVT_APP_KEY */
ntg_listenable* ntg_loop_get_listenable(ntg_loop* loop);

#endif // _NTG_LOOP_H_
