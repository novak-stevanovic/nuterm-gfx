#include <assert.h>
#include <stdio.h>

#include "ntg.h"
#include "core/ntg_stage.h"
#include "pthread.h"
#include "nt.h"

#include "shared/ntg_log.h"

/* -------------------------------------------------------------------------- */

static struct ntg_xy _app_size = NTG_XY_UNSET;

static pthread_t _ntg_thread;
static bool _launched = false;
static void (*_gui_fn)(void* data) = NULL; 
static void* _gui_fn_data = NULL;

static bool _loop = false;

static void* _ntg_thread_func(void* data);

static ntg_stage* _stage = NULL;

/* -------------------------------------------------------------------------- */

void ntg_initialize(ntg_stage* stage, ntg_gui_fn gui_fn, void* data)
{
    assert(stage != NULL);
    assert(gui_fn != NULL);

    __ntg_log_init__("ntg_log.txt");

    _gui_fn = gui_fn;
    _gui_fn_data = data;
    _stage = stage;

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

void ntg_launch()
{
    nt_alt_screen_enable(NULL);
    nt_cursor_hide(NULL);

    int status = pthread_create(&_ntg_thread, NULL, _ntg_thread_func, _gui_fn_data);
    if(status != 0) assert(0);
}

void* ntg_destroy()
{
    void* _data;
    pthread_join(_ntg_thread, &_data);

    nt_alt_screen_disable(NULL);
    nt_cursor_show(NULL);
    __nt_deinit__();

   // __ntg_log_deinit__();

    return _data;
}

struct ntg_xy ntg_get_size()
{
    return _app_size;
}

/* -------------------------------------------------------------------------- */

void ntg_loop(uint framerate)
{
    nt_status _status;
    struct nt_event event;
    uint timeout = NTG_WAIT_TIMEOUT(framerate); 
    _loop = true;
    while(_loop)
    {
        event = nt_wait_for_event(timeout, &_status);

        timeout = NTG_WAIT_TIMEOUT(framerate) - event.elapsed;

        size_t _width, _height;
        ntg_stage_status status;
        switch(event.type)
        {
            case NT_EVENT_TYPE_KEY:
                status = ntg_stage_feed_key_event(_stage, event.key_data);
                if(status == NTG_STAGE_QUIT)
                    _loop = false;
                break;
            case NT_EVENT_TYPE_RESIZE:
                nt_get_term_size(&_width, &_height);
                _app_size = ntg_xy(_width, _height);
                break;
            case NT_EVENT_TYPE_TIMEOUT:
                ntg_stage_render(_stage, _app_size);
                break;
        }

        assert(_status == NT_SUCCESS);
    }

}

static void* _ntg_thread_func(void* data)
{
    _launched = true;

    _gui_fn(_gui_fn_data);

    return NULL;
}

ntg_stage* ntg_get_stage()
{
    return _stage;
}
