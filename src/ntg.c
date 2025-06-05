#include <assert.h>
#include <stdio.h>

#include "ntg.h"
#include "core/ntg_scene.h"
#include "core/ntg_stage.h"
#include "object/ntg_color_block.h"
#include "shared/_ntg_shared.h"
#include "pthread.h"
#include "nt.h"

static pthread_t _ntg_thread;
static ntg_init_gui_fn _init_gui_func = NULL; 
static void* _init_gui_func_data = NULL;

#define NTG_TIMEOUT 16

static void* _ntg_thread_func(void* data)
{
    _init_gui_func(_init_gui_func_data);

    ntg_color_block_t* cb = ntg_color_block_new(nt_color_new(255, 0, 255));

    ntg_scene_t* s = ntg_scene_new();
    ntg_scene_set_root(s, (ntg_object_t*)cb);

    ntg_stage_set_scene(s);

    ntg_stage_render();

    char c;
    while(1)
    {
        c = getchar();
        if(c == 'q')
            break;
    }

    // nt_status_t _status;
    // struct nt_event event;
    // while(true)
    // {
    //     event = nt_wait_for_event(NTG_TIMEOUT, &_status);
    //
    //     if(event.key_data.esc_key_data.esc_key == NT_ESC_KEY_F5)
    //         break;
    //
    //     assert(_status == NT_SUCCESS);
    // }

    ntg_color_block_destroy(cb);
    ntg_scene_destroy(s);

    return NULL;
}

void ntg_launch(void (*init_gui_fn)(void* data), void* data,
        ntg_status_t* out_status)
{
    if(init_gui_fn == NULL)
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

    _init_gui_func = init_gui_fn;
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

    __nt_deinit__();

    __ntg_stage_deinit__();

    return _data;
}
