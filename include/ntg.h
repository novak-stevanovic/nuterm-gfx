#ifndef _NTG_H_
#define _NTG_H_

#include "nt_event.h"
#include "nt_gfx.h"

#include "core/loop/ntg_loop.h"
#include "core/loop/ntg_taskmaster.h"
#include "core/renderer/ntg_def_renderer.h"
#include "core/stage/ntg_def_stage.h"
#include "core/scene/ntg_def_scene.h"
#include "core/object/ntg_object.h"

/* -------------------------------------------------------------------------- */
/* INIT/DEINIT */
/* -------------------------------------------------------------------------- */

void _ntg_init_();
void _ntg_deinit_();

/* -------------------------------------------------------------------------- */
/* LAUNCH */
/* -------------------------------------------------------------------------- */

typedef void (*ntg_gui_fn)(void* data);

void ntg_launch(ntg_gui_fn gui_fn, void* data);

/* Waits for NTG thread to finish.
 * This function returns the return value of `gui_fn` passed in ntg_launch(). */
void* ntg_wait();

/* -------------------------------------------------------------------------- */
/* KICKSTART */
/* -------------------------------------------------------------------------- */

struct ntg_kickstart_obj
{
    ntg_entity_group* group;

    ntg_loop* loop;

    ntg_taskmaster* taskmaster;

    ntg_def_renderer* renderer;
};

struct ntg_kickstart_obj ntg_kickstart(
        ntg_stage* init_stage,
        unsigned int loop_framerate, /* non-zero */
        ntg_loop_process_event_fn loop_process_event_fn,
        void* loop_data, void* renderer_data,
        ntg_entity_system* system);
void ntg_kickstart_end(struct ntg_kickstart_obj* obj);

/* -------------------------------------------------------------------------- */
/* TO-DO LIST */
/* -------------------------------------------------------------------------- */

// TODO: include ntg_entity_type.h in ntg_entity.h ?
// TODO: Rethink protected/internal/read-only API(including void* data field)
// TODO: Rethink nullable argument in fns
// TODO: ntg_border_box: ntg_object_get_group_root()...

// TODO: ntg_event_dlgt: unsub, destroy order? }------------------------|
// TODO: ntg_object_container: rethink - object register? }-------------|
// TODO: global: add ntg_allocator for faster object initialization? }--| **
// TODO: ntg_object -> ntg_element, ntg_entity? }-----------------------|
// TODO: ntg_event - listenable, delegate rework? }---------------------|
// --------------------------------------------------------------
// TODO: ntg_taskmaster: what if a task gets stuck, what if deinit func is called before a task is finished? | *
// TODO: ntg_list and/or ntg_table: implement | **
// TODO: ntg_scene: implement multi-root, multifocused? system | **
// TODO: ntg_scene: optimize ntg_def_scene layout fn | **
// TODO: nuterm: add signal event, maybe more event types - allow user to define events? | *
// TODO: add convenience macros | *
// TODO: add inheritable constructors | *
// TODO: add proper error-handling | ***

#endif // _NTG_H_
