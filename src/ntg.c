#include <assert.h>
#include <stdio.h>

#include "ntg.h"
#include "base/ntg_event.h"
#include "core/ntg_stage.h"
#include "shared/_ntg_shared.h"
#include "pthread.h"
#include "nt.h"

#include "object/ntg_color_block.h"
#include "shared/ntg_log.h"

extern ntg_color_block* cb1;
extern ntg_object* _cb1;

/* -------------------------------------------------------------------------- */

static pthread_t _ntg_thread;
static bool _launched = false;
static void (*_gui_fn)(void* data) = NULL; 
static void* _gui_fn_data = NULL;

static bool _loop = false;

static void* _ntg_thread_func(void* data);

static ntg_event* _key_event = NULL;
static ntg_event* _resize_event = NULL;

/* -------------------------------------------------------------------------- */

void ntg_initialize(ntg_gui_fn gui_fn, void* data)
{
    assert(gui_fn != NULL);

    __ntg_log_init__("ntg_log.txt");

    _gui_fn = gui_fn;
    _gui_fn_data = data;

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

    __ntg_stage_init__();
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

    __ntg_stage_deinit__();

   // __ntg_log_deinit__();

    return _data;
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

        switch(event.type)
        {
            case NT_EVENT_TYPE_KEY:
                ntg_stage_feed_key_event(event.key_data);
                break;
            case NT_EVENT_TYPE_RESIZE:
                ntg_stage_feed_resize_event(event.resize_data);
                break;
            case NT_EVENT_TYPE_TIMEOUT:
                ntg_stage_render();
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
