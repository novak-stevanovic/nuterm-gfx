#include <assert.h>

#include "ntg.h"
#include "core/ntg_stage.h"
#include "shared/_ntg_shared.h"
#include "pthread.h"
#include "nt.h"

/* -------------------------------------------------------------------------- */

static pthread_t _ntg_thread;
static void (*_gui_fn)(void* data) = NULL; 
static void* _init_gui_func_data = NULL;

#define FPS 30
#define TIMEOUT (1000000.0 / FPS)

static void* _ntg_thread_func(void* data);

static bool _use_ntg_loop = true;
static void _ntg_loop();

/* -------------------------------------------------------------------------- */

static void _launch(void (*gui_fn)(void* data), void* data,
        ntg_status_t* out_status)
{
    if(gui_fn == NULL)
        _vreturn(out_status, NTG_ERR_INVALID_ARG);

    nt_status_t _status;
    __nt_init__(&_status);
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

void ntg_launch(void (*init_gui_fn)(void* data), void* data,
        ntg_status_t* out_status)
{
    _use_ntg_loop = true;
    _launch(init_gui_fn, data, out_status);
}

void ntg_launch_(ntg_gui_run_fn gui_fn, void* data,
        ntg_status_t* out_status)
{
    _use_ntg_loop = false;
    _launch(gui_fn, data, out_status);
}

void* ntg_destroy()
{
    void* _data;
    pthread_join(_ntg_thread, &_data);

    __nt_deinit__();

    __ntg_stage_deinit__();

    return _data;
}

/* -------------------------------------------------------------------------- */

static void _ntg_loop()
{
    nt_status_t _status;
    struct nt_event event;
    uint timeout = TIMEOUT;
    while(true)
    {
        event = nt_wait_for_event(timeout, &_status);
        timeout = TIMEOUT - event.elapsed;

        if(event.key_data.esc_key_data.esc_key == NT_ESC_KEY_F5)
            break;

        assert(_status == NT_SUCCESS);
    }

}

static void* _ntg_thread_func(void* data)
{
    _gui_fn(_init_gui_func_data);

    if(_use_ntg_loop) _ntg_loop();

    return NULL;
}
