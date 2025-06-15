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

extern ntg_color_block_t* cb1;
extern ntg_object_t* _cb1;

/* -------------------------------------------------------------------------- */

static pthread_t _ntg_thread;
static void (*_gui_fn)(void* data) = NULL; 
static void* _init_gui_func_data = NULL;

#define FRAMERATE 30
#define TIMEOUT (1000.0 / FRAMERATE)

static void* _ntg_thread_func(void* data);

/* -------------------------------------------------------------------------- */

void ntg_launch(void (*gui_fn)(void* data), void* data,
        ntg_status_t* out_status)
{
    if(gui_fn == NULL)
        _vreturn(out_status, NTG_ERR_INVALID_ARG);

   // __ntg_log_init__(NULL);

    nt_status_t _status;
    __nt_init__(&_status);
    nt_alt_screen_enable(&_status);
    nt_cursor_hide(NULL);

    __ntg_stage_init__();
    switch(_status)
    {
        // TODO handle other cases
        case NT_SUCCESS:
            break;
        case NT_ERR_ALLOC_FAIL:
            _vreturn(out_status, NTG_ERR_UNEXPECTED);
        default:
            _vreturn(out_status, NTG_ERR_UNEXPECTED);
    }

    _gui_fn = gui_fn;
    _init_gui_func_data = data;

    int status = pthread_create(&_ntg_thread, NULL, _ntg_thread_func, data);
    if(status != 0)
        _vreturn(out_status, NTG_ERR_UNEXPECTED);

    _vreturn(out_status, NTG_SUCCESS);
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

void ntg_loop()
{
    nt_status_t _status;
    struct nt_event event;
    uint timeout = TIMEOUT;
    while(true)
    {
        event = nt_wait_for_event(timeout, &_status);

        timeout = TIMEOUT - event.elapsed;

        if(event.type == NT_EVENT_TYPE_KEY)
        {
            if(event.key_data.type == NT_KEY_EVENT_UTF32)
            {
                if(event.key_data.utf32_data.codepoint == 'q')
                    break;
            }
            else // ESCAPE
            {
                if(event.key_data.esc_key_data.esc_key == NT_ESC_KEY_ARR_UP)
                {
                    _ntg_object_scroll(_cb1, ntg_dxy(0, -1));
                }
                if(event.key_data.esc_key_data.esc_key == NT_ESC_KEY_ARR_RIGHT)
                {
                    _ntg_object_scroll(_cb1, ntg_dxy(1, 0));
                }
                if(event.key_data.esc_key_data.esc_key == NT_ESC_KEY_ARR_DOWN)
                {
                    _ntg_object_scroll(_cb1, ntg_dxy(0, 1));
                }
                if(event.key_data.esc_key_data.esc_key == NT_ESC_KEY_ARR_LEFT)
                {
                    _ntg_object_scroll(_cb1, ntg_dxy(-1, 0));
                }
            }
        }
        else if(event.type == NT_EVENT_TYPE_RESIZE)
        {
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
    _gui_fn(_init_gui_func_data);

    return NULL;
}
