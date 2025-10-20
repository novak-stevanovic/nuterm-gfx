#ifndef _NTG_STAGE_H_
#define _NTG_STAGE_H_

#include "core/scene/ntg_scene.h"
#include "nt_event.h"

typedef struct ntg_stage ntg_stage;
typedef struct nt_charbuff nt_charbuff;

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

#define NTG_STAGE(stg_ptr) ((ntg_stage*)(stg_ptr))

/* Abstract */
struct ntg_stage
{
    ntg_scene* __active_scene;
    struct ntg_xy __size;

    ntg_stage_process_key_fn __process_key_fn;

    nt_charbuff* __buff;
    ntg_rcell_vgrid __back_buffer;

    ntg_listenable __listenable;
};

/* If `process_key_fn` is NULL, a default value will be used.
 * The default function would exit the program on `q` and would delegate
 * and key events to the active scene */
void __ntg_stage_init__(ntg_stage* stage,
        ntg_stage_process_key_fn process_key_fn);

void __ntg_stage_deinit__(ntg_stage* stage);

ntg_scene* ntg_stage_get_scene(ntg_stage* stage);
void ntg_stage_set_scene(ntg_stage* stage, ntg_scene* scene);

struct ntg_xy ntg_stage_get_size(const ntg_stage* stage);
void ntg_stage_set_size(ntg_stage* stage, struct ntg_xy size);

void ntg_stage_render(ntg_stage* stage, ntg_stage_render_mode render_mode);

/* Calls used ntg_stage_process_key_fn. */
ntg_stage_status ntg_stage_feed_key_event(ntg_stage* stage,
        struct nt_key_event key_event);

void ntg_stage_listen(ntg_stage* stage, struct ntg_event_sub sub);
void ntg_stage_stop_listening(ntg_stage* stage, void* subscriber);

ntg_listenable* _ntg_stage_get_listenable(ntg_stage* stage);

#endif // _NTG_STAGE_H_
