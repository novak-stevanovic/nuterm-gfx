#include <assert.h>
#include <stdio.h>

#include "ntg.h"
#include "core/ntg_stage.h"
#include "shared/_ntg_shared.h"
#include "pthread.h"
#include "nt.h"

#include "object/ntg_color_block.h"
#include "shared/ntg_log.h"

// TODO: remove
#include "object/def/ntg_object_def.h"

extern ntg_color_block* cb1;
extern ntg_object* _cb1;

/* -------------------------------------------------------------------------- */

static pthread_t _ntg_thread;
static bool _launched = false;
static void (*_gui_fn)(void* data) = NULL; 
static void* _gui_fn_data = NULL;

#define FRAMERATE_DEFAULT 60
static uint _framerate;

#define TIMEOUT(fps) (1000.0 / fps)

static void* _ntg_thread_func(void* data);

/* -------------------------------------------------------------------------- */

void ntg_initialize(ntg_gui_fn gui_fn, void* data)
{
    assert(gui_fn != NULL);

    __ntg_log_init__("ntg_log.txt");

    _gui_fn = gui_fn;
    _gui_fn_data = data;
    _framerate = FRAMERATE_DEFAULT;

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

void ntg_set_framerate(uint framerate)
{
    if(!_launched)
        _framerate = framerate;
    else assert(0);
}

/* -------------------------------------------------------------------------- */

void ntg_loop()
{
    nt_status _status;
    struct nt_event event;
    uint timeout = TIMEOUT(_framerate);
    bool loop = true;
    while(loop)
    {
        event = nt_wait_for_event(timeout, &_status);

        timeout = TIMEOUT(_framerate) - event.elapsed;

        if(event.type == NT_EVENT_TYPE_KEY)
        {
            bool processed = _ntg_stage_process_key_evnet(event.key_data);

            if(!processed)
            {
                if(event.key_data.type == NT_KEY_EVENT_UTF32)
                {
                    switch(event.key_data.utf32_data.codepoint)
                    {
                        case 'q':
                            loop = false;
                            break;
                    }
                }
            }
        }
        else if(event.type == NT_EVENT_TYPE_RESIZE)
        {
            ntg_stage_feed_resize_event(event.resize_data);
        }
        else // TIMEOUT
        {
            ntg_stage_render();
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
