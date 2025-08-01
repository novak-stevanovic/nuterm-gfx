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

typedef enum ntg_stage_render_mode
{
    NTG_STAGE_RENDER_MODE_FULL,
    NTG_STAGE_RENDER_MODE_OPTIMIZED
} ntg_stage_render_mode;

typedef ntg_stage_status (*ntg_stage_process_key_fn)(ntg_stage* stage,
        struct nt_key_event key_event);

typedef void (*ntg_stage_render_fn)(ntg_stage* stage,
        ntg_stage_render_mode render_mode);

#define NTG_STAGE(stg_ptr) ((ntg_stage*)(stg_ptr))

/* Abstract */
struct ntg_stage
{
    ntg_scene* _active_scene;
    struct ntg_xy _size;

    ntg_listenable __listenable;

    ntg_stage_process_key_fn __process_key_fn;
    ntg_stage_render_fn __render_fn;
};

void __ntg_stage_init__(ntg_stage* stage,
        ntg_stage_render_fn render_fn);
void __ntg_stage_deinit__(ntg_stage* stage);

void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

void ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size);

/* Calls used ntg_stage_render_fn. */
void ntg_stage_render(ntg_stage* stage, ntg_stage_render_mode render_mode);

/* Calls used ntg_stage_process_key_fn. */
ntg_stage_status ntg_stage_feed_key_event(ntg_stage* stage,
        struct nt_key_event key_event);

/* By default, the stage will feed the key event to the active scene.
 * Tell the stage to shut down by pressing 'q'. */
void _ntg_stage_set_process_key_fn(ntg_stage* stage,
        ntg_stage_process_key_fn process_key_fn);

void ntg_stage_listen(ntg_stage* stage, struct ntg_event_sub sub);
void ntg_stage_stop_listening(ntg_stage* stage, void* subscriber);

/* Event types raised by ntg_stage:
 * 1) NTG_ETYPE_STAGE_RESIZE,
 * 2) NTG_ETYPE_STAGE_SCENE_CHANGE,
 * 3) NTG_ETYPE_STAGE_RENDER
 */
ntg_listenable* _ntg_stage_get_listenable(ntg_stage* stage);

#endif // _NTG_STAGE_H_
