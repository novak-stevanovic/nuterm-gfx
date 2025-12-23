#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "ntg.h"
#include "base/entity/_ntg_entity_system.h"
#include "nt.h"
#include "shared/ntg_log.h"
#include "shared/sarena.h"

/* -------------------------------------------------------------------------- */

static pthread_t __ntg_thread;
static bool __launched = false;

static void* ntg_thread_fn(void* _thread_fn_data);

/* -------------------------------------------------------------------------- */

void _ntg_init_()
{
    _ntg_log_init_("ntg_log.txt");

    nt_status _status;
    __nt_init__(&_status);

    switch(_status)
    {
        // TODO handle other cases
        case NT_SUCCESS:
            break;
        default:
            assert(0);
    }
}

struct ntg_app_thread_fn_data
{
    ntg_gui_fn gui_fn;
    void* gui_fn_data;
};

void ntg_launch(ntg_gui_fn gui_fn, void* data)
{
    assert(__launched == false);
    if(gui_fn == NULL) return;

    nt_alt_screen_enable(NULL);
    nt_cursor_hide(NULL);

    struct ntg_app_thread_fn_data* thread_fn_data = malloc(
            sizeof(struct ntg_app_thread_fn_data));
    assert(thread_fn_data != NULL);

    thread_fn_data->gui_fn = gui_fn;
    thread_fn_data->gui_fn_data = data;

    int status = pthread_create(&__ntg_thread, NULL,
            ntg_thread_fn, thread_fn_data);

    assert(status == 0);

    __launched = true;
}

void* ntg_wait()
{
    if(!__launched) return NULL;

    void* _data;
    pthread_join(__ntg_thread, &_data);
    
    return _data;
}

void _ntg_deinit_()
{
    nt_cursor_show(NULL);
    nt_alt_screen_disable(NULL);
    __nt_deinit__();

   _ntg_log_deinit_();
}

static void* ntg_thread_fn(void* _thread_fn_data)
{
    struct ntg_app_thread_fn_data* data =
        (struct ntg_app_thread_fn_data*)_thread_fn_data;

    ntg_entity_system* es = ntg_entity_system_new();

    data->gui_fn(es, data->gui_fn_data);

    ntg_entity_system_destroy(es);

    free(data);

    return NULL;
}

void ntg_kickstart(ntg_stage* init_stage,
        unsigned int loop_framerate, /* non-zero */
        ntg_loop_process_event_fn loop_process_event_fn,
        ntg_entity_group* group,
        ntg_entity_system* system,
        struct ntg_kickstart_obj* out_obj)
{
    assert(loop_framerate > 0);
    assert(loop_framerate < 1000);
    assert(group != NULL);
    assert(system != NULL);
    assert(out_obj != NULL);

    ntg_loop* loop = &out_obj->loop;
    ntg_def_renderer* renderer = &out_obj->renderer;
    ntg_taskmaster* taskmaster = &out_obj->taskmaster;

    _ntg_taskmaster_init_(taskmaster, group, system);

    _ntg_def_renderer_init_(renderer, loop, group, system);

    struct ntg_loop_init_data loop_init_data = {
        .taskmaster = taskmaster,
        .renderer = (ntg_renderer*)renderer,
        .framerate = loop_framerate,
        .process_event_fn = loop_process_event_fn,
        .init_stage = init_stage
    };

    _ntg_loop_init_(loop, loop_init_data, group, system);
}
