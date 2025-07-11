#ifndef _NTG_STAGE_H_
#define _NTG_STAGE_H_

#include "core/ntg_scene.h"
#include "nt_event.h"

typedef struct ntg_stage ntg_stage;

typedef enum ntg_stage_status
{
    NTG_STAGE_CONTINUE,
    NTG_STAGE_QUIT
} ntg_stage_status;

typedef ntg_stage_status (*ntg_stage_process_key_fn)(ntg_stage* stage,
        struct nt_key_event key_event);

/* Default: Dispatches the key event to active scene. */
ntg_stage_status ntg_stage_process_key_fn_default(ntg_stage* stage,
        struct nt_key_event key_event);

typedef void (*ntg_stage_render_fn)(ntg_stage* stage, struct ntg_xy size);

#define NTG_STAGE(stg_ptr) ((ntg_stage*)(stg_ptr))

/* Abstract */
struct ntg_stage
{
    ntg_scene* _active_scene;
    ntg_stage_process_key_fn __process_key_fn;
    ntg_stage_render_fn __render_fn;
};

void __ntg_stage_init__(ntg_stage* stage,
        ntg_stage_render_fn render_fn);

void __ntg_stage_deinit__(ntg_stage* stage);

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

/* Calls used ntg_stage_render_fn. */
void ntg_stage_render(ntg_stage* stage, struct ntg_xy size);

/* Calls used ntg_stage_process_key_fn. */
ntg_stage_status ntg_stage_feed_key_event(ntg_stage* stage,
        struct nt_key_event key_event);

void _ntg_stage_set_process_key_fn(ntg_stage* stage,
        ntg_stage_process_key_fn process_key_fn);

#endif // _NTG_STAGE_H_
