#include "ntg.h"
#include "nt.h"
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include "core/entity/_ntg_entity_system.h"
#include "core/loop/_ntg_loop.h"

#define UCONV_IMPLEMENTATION
#include "thirdparty/uconv.h"

#define SARENA_IMPLEMENTATION
#include "thirdparty/sarena.h"

#define GENC_IMPLEMENTATION
#include "thirdparty/genc.h"

/* -------------------------------------------------------------------------- */

static pthread_t ntg_thread;
static bool launched = false;

static void* ntg_thread_fn(void* _data);

/* -------------------------------------------------------------------------- */

void ntg_init()
{
    ntg_log_init("ntg_log.txt");

    nt_status _status;
    nt_init(&_status);

    switch(_status)
    {
        // TODO handle other cases
        case NT_SUCCESS:
            break;
        default:
            assert(0);
    }
}

struct thread_fn_data
{
    ntg_gui_fn gui_fn;
    void* gui_fn_data;
};

void ntg_launch(ntg_gui_fn gui_fn, void* data)
{
    assert(launched == false);
    if(gui_fn == NULL) return;

    nt_alt_screen_enable(NULL);
    nt_cursor_hide(NULL);

    struct thread_fn_data* thread_fn_data = malloc(sizeof(struct thread_fn_data));
    assert(thread_fn_data != NULL);

    thread_fn_data->gui_fn = gui_fn;
    thread_fn_data->gui_fn_data = data;

    int status = pthread_create(&ntg_thread, NULL, ntg_thread_fn, thread_fn_data);

    assert(status == 0);

    launched = true;
}

void ntg_wait()
{
    if(!launched) return;

    pthread_join(ntg_thread, NULL);
}

void ntg_deinit()
{
    nt_cursor_show(NULL);
    nt_alt_screen_disable(NULL);
    nt_deinit();

   ntg_log_deinit();
}

static void* ntg_thread_fn(void* _data)
{
    struct thread_fn_data* data = (struct thread_fn_data*)_data;

    ntg_entity_system* es = _ntg_entity_system_new();

    ntg_loop* loop = _ntg_loop_new(es);

    data->gui_fn(es, loop, data->gui_fn_data);

    _ntg_entity_system_destroy(es);

    free(data);

    return NULL;
}
