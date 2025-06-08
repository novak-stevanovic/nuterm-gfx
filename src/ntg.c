#include <assert.h>
#include <stdio.h>

#include "ntg.h"
#include "core/ntg_stage.h"
#include "shared/_ntg_shared.h"
#include "pthread.h"
#include "nt.h"

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

        ntg_stage_render();

        if((event.type == NT_EVENT_TYPE_KEY) &&
                (event.key_data.esc_key_data.esc_key == NT_ESC_KEY_F5))
            break;

        assert(_status == NT_SUCCESS);
    }

}

static void* _ntg_thread_func(void* data)
{
    _gui_fn(_init_gui_func_data);

    return NULL;
}
