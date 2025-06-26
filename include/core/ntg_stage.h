#ifndef _NTG_STAGE_H_
#define _NTG_STAGE_H_

#include "core/ntg_scene.h"
#include "nt_event.h"

typedef struct ntg_stage ntg_stage;

typedef bool (*ntg_stage_process_key_fn)(ntg_stage* stage,
        struct nt_key_event key_event);

typedef void (*ntg_stage_process_resize_fn)(ntg_stage* stage,
        struct nt_resize_event resize_event);

typedef void (*ntg_stage_render_fn)(ntg_stage* stage);

struct ntg_stage
{
    ntg_scene* _active_scene;
    ntg_stage_process_key_fn __process_key_fn;
    ntg_stage_process_resize_fn __process_resize_fn;
    ntg_stage_render_fn __render_fn;
};

/* Dispatches the key event to active scene */
bool ntg_stage_process_key_fn_def(ntg_stage* stage,
        struct nt_key_event key_event);

/* Dispatches the resize event to active scene */
void ntg_stage_process_resize_fn_def(ntg_stage* stage,
        struct nt_resize_event resize_event);

void __ntg_stage_init__(ntg_stage* stage,
        ntg_stage_process_key_fn process_key_fn,
        ntg_stage_process_resize_fn process_resize_fn,
        ntg_stage_render_fn render_fn);

void __ntg_stage_deinit__(ntg_stage* stage);

ntg_scene* ntg_stage_get_scene(ntg_stage* stage);
void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

/* Calls used ntg_stage_render_fn */
void ntg_stage_render(ntg_stage* stage);

/* Calls used ntg_stage_process_key_fn */
bool ntg_stage_feed_key_event(ntg_stage* stage, struct nt_key_event key_event);
/* Calls used ntg_stage_process_resize_fn */
void ntg_stage_feed_resize_event(ntg_stage* stage,
        struct nt_resize_event resize_event);

#endif // _NTG_STAGE_H_
