#ifndef _NTG_H_
#define _NTG_H_

#include "nt_event.h"
#include "nt_gfx.h"

// TODO: ntg_event_dlgt: unsubscribe then destroy | *
// TODO: ntg_taskmaster: what if a task gets stuck, what if deinit func is called before a task is finished? | *
// TODO: ntg_object: ntg_object_get_children() returns const ntg_object_vec* | *
// TODO: global: add ntg_allocator for faster object initialization? | *
// TODO: ntg_list and/or ntg_table: implement | **
// TODO: ntg_scene: implement multi-root system | **
// TODO: ntg_scene: optimize ntg_def_scene layout fn | **
// TODO: nuterm: add signal event, maybe more event types? | *
// TODO: add convenience macros | *
// TODO: add proper error-handling | ***

/* -------------------------------------------------------------------------- */

void _ntg_init_();
void _ntg_deinit_();

/* -------------------------------------------------------------------------- */

typedef void (*ntg_gui_fn)(void* data);

void ntg_launch(ntg_gui_fn gui_fn, void* data);

/* Waits for NTG thread to finish.
 * This function returns the return value of `gui_fn` passed in nui_launch(). */
void* ntg_wait();

/* -------------------------------------------------------------------------- */

#include "core/object/ntg_object.h"
#include "core/loop/ntg_loop.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/stage/ntg_def_stage.h"
#include "core/scene/ntg_def_scene.h"

struct ntg_kickstart_obj
{
    ntg_loop* loop;

    ntg_taskmaster* taskmaster;

    ntg_def_renderer* renderer;
    ntg_renderer* _renderer;
};

struct ntg_kickstart_obj ntg_kickstart(
        ntg_stage* init_stage,
        unsigned int loop_framerate, /* non-zero */
        ntg_loop_process_event_fn loop_process_event_fn,
        void* loop_data, void* renderer_data);
void ntg_kickstart_end(struct ntg_kickstart_obj* obj);

#endif // _NTG_H_
