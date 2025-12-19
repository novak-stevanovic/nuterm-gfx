#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "ntg.h"
#include "nt.h"
#include "shared/ntg_log.h"

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

    if(data->gui_fn != NULL) data->gui_fn(data->gui_fn_data);

    free(data);

    return NULL;
}

struct ntg_kickstart_obj ntg_kickstart(
        ntg_stage* init_stage,
        unsigned int loop_framerate, /* non-zero */
        ntg_loop_process_event_fn loop_process_event_fn,
        void* loop_data, void* renderer_data)
{
    assert(loop_framerate > 0);
    assert(loop_framerate < 1000);
    // assert(loop_process_key_fn != NULL);

    ntg_loop* loop = (ntg_loop*)malloc(sizeof(ntg_loop));
    ntg_loop* _loop = (ntg_loop*)loop;
    assert(loop != NULL);

    ntg_def_renderer* renderer = (ntg_def_renderer*)malloc(sizeof(ntg_def_renderer));
    ntg_renderer* _renderer = (ntg_renderer*)renderer;
    assert(renderer != NULL);

    ntg_taskmaster* taskmaster = ntg_taskmaster_new();
    
    _ntg_loop_init_(
            loop,
            loop_process_event_fn,
            init_stage,
            _renderer,
            taskmaster,
            loop_framerate,
            loop_data);

    _ntg_def_renderer_init_(renderer, _loop, renderer_data);

    return (struct ntg_kickstart_obj) {
        .renderer = renderer,
        ._renderer = _renderer,

        .taskmaster = taskmaster,

        .loop = loop,
    };
}

void ntg_kickstart_end(struct ntg_kickstart_obj* obj)
{
    assert(obj != NULL);

    _ntg_loop_deinit_(obj->loop);
    free(obj->loop);

    _ntg_def_renderer_deinit_(obj->renderer);
    free(obj->renderer);

    ntg_taskmaster_destroy(obj->taskmaster);

    (*obj) = (struct ntg_kickstart_obj) {0};
}
