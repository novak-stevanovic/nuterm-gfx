#ifndef _NTG_STAGE_H_
#define _NTG_STAGE_H_

#include "core/ntg_scene.h"
#include "nt_event.h"

typedef bool (*ntg_stage_process_key_event_fn)(struct nt_key_event key_event);
typedef void (*ntg_stage_process_resize_event_fn)(struct nt_resize_event resize_event);
typedef void (*ntg_stage_perform_render_fn)();

/* Pass NULL to use default functions */
void __ntg_stage_init__(ntg_stage_process_key_event_fn on_key_event_fn,
        ntg_stage_process_resize_event_fn on_resize_event_fn,
        ntg_stage_perform_render_fn on_render_fn);

void __ntg_stage_deinit__();

ntg_scene* ntg_stage_get_scene();
void ntg_stage_set_scene(ntg_scene* scene);

void ntg_stage_render();

/* Used internally in ntg_loop(). May be used in a custom loop. */
bool ntg_stage_feed_key_event(struct nt_key_event key_event);
void ntg_stage_feed_resize_event(struct nt_resize_event resize_event);

#endif // _NTG_STAGE_H_
