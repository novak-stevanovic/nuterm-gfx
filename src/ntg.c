#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "ntg.h"
#include "nt.h"
#include "shared/ntg_log.h"

#define _UCONV_IMPLEMENTATION_
#include "shared/_uconv.h"
#undef _UCONV_IMPLEMENTATION_

/* -------------------------------------------------------------------------- */

static pthread_t __ntg_thread;
static bool __launched = false;

static void* __ntg_thread_fn(void* _thread_fn_data);

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
            __ntg_thread_fn, thread_fn_data);

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

void __ntg_deinit__()
{
    nt_cursor_show(NULL);
    nt_alt_screen_disable(NULL);
    __nt_deinit__();

   __ntg_log_deinit__();
}

static void* __ntg_thread_fn(void* _thread_fn_data)
{
    struct ntg_app_thread_fn_data* data =
        (struct ntg_app_thread_fn_data*)_thread_fn_data;

    if(data->gui_fn != NULL) data->gui_fn(data->gui_fn_data);

    free(data);

    return NULL;
}
