#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ntg.h"
#include "core/ntg_stage.h"
#include "pthread.h"
#include "nt.h"

#include "shared/ntg_log.h"

/* -------------------------------------------------------------------------- */

static struct ntg_xy _app_size = NTG_XY_UNSET;
static bool _resize = true;

static pthread_t _ntg_thread;
static bool _launched = false;

static void* _ntg_thread_fn(void* _thread_fn_data);

/* -------------------------------------------------------------------------- */

void __ntg_init__()
{
    __ntg_log_init__("ntg_log.txt");

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

    size_t _width, _height;
    nt_get_term_size(&_width, &_height);
    _app_size = ntg_xy(_width, _height);
}

struct ntg_thread_fn_data
{
    ntg_gui_fn gui_fn;
    ntg_stage* stage;
    void* gui_fn_data;
};

void ntg_launch(ntg_stage* main_stage, ntg_gui_fn gui_fn, void* data)
{
    assert(_launched == false);

    nt_alt_screen_enable(NULL);
    nt_cursor_hide(NULL);

    struct ntg_thread_fn_data* thread_fn_data = malloc(
            sizeof(struct ntg_thread_fn_data));
    assert(thread_fn_data != NULL);

    thread_fn_data->gui_fn = gui_fn;
    thread_fn_data->stage = main_stage;
    thread_fn_data->gui_fn_data = data;

    int status = pthread_create(&_ntg_thread, NULL,
            _ntg_thread_fn, thread_fn_data);

    assert(status == 0);

    _launched = true;
}

void* ntg_wait()
{
    if(!_launched) return NULL;

    void* _data;
    pthread_join(_ntg_thread, &_data);
    
    return _data;
}

void __ntg_deinit__()
{
    nt_cursor_show(NULL);
    nt_alt_screen_disable(NULL);
    __nt_deinit__();

   __ntg_log_deinit__();
}

struct ntg_xy ntg_get_size()
{
    return _app_size;
}

/* -------------------------------------------------------------------------- */

void ntg_loop(ntg_stage* main_stage, uint framerate)
{
    nt_status _status;
    struct nt_event event;
    uint timeout = NTG_WAIT_TIMEOUT(framerate); 
    bool loop = true;
    while(loop)
    {
        event = nt_wait_for_event(timeout, &_status);

        timeout = NTG_WAIT_TIMEOUT(framerate) - event.elapsed;

        size_t _width, _height;
        ntg_stage_status status;
        ntg_stage_render_mode render_mode;
        switch(event.type)
        {
            case NT_EVENT_TYPE_KEY:
                status = ntg_stage_feed_key_event(main_stage, event.key_data);
                if(status == NTG_STAGE_QUIT)
                    loop = false;
                break;
            case NT_EVENT_TYPE_RESIZE:
                nt_get_term_size(&_width, &_height);
                _app_size = ntg_xy(_width, _height);
                _resize = true;
                break;
            case NT_EVENT_TYPE_TIMEOUT:
                render_mode = _resize ?
                    NTG_STAGE_RENDER_MODE_FULL :
                    NTG_STAGE_RENDER_MODE_OPTIMIZED;
                _resize = false;
                ntg_stage_set_size(main_stage, _app_size);
                ntg_stage_render(main_stage, render_mode);
                break;
        }

        assert(_status == NT_SUCCESS);
    }

}

static void* _ntg_thread_fn(void* _thread_fn_data)
{
    struct ntg_thread_fn_data* data =
        (struct ntg_thread_fn_data*)_thread_fn_data;

    if(data->gui_fn != NULL)
        data->gui_fn(data->stage, data->gui_fn_data);

    free(data);

    return NULL;
}
