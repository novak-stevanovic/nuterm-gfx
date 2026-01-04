#include "ntg.h"
#include "nt.h"
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include "core/entity/_ntg_entity_system.h"

static ntg_loop* ntg_loop_new(ntg_entity_system* system);

/* -------------------------------------------------------------------------- */

static pthread_t __ntg_thread;
static bool __launched = false;

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
    assert(__launched == false);
    if(gui_fn == NULL) return;

    nt_alt_screen_enable(NULL);
    nt_cursor_hide(NULL);

    struct thread_fn_data* thread_fn_data = malloc(sizeof(struct thread_fn_data));
    assert(thread_fn_data != NULL);

    thread_fn_data->gui_fn = gui_fn;
    thread_fn_data->gui_fn_data = data;

    int status = pthread_create(&__ntg_thread, NULL, ntg_thread_fn, thread_fn_data);

    assert(status == 0);

    __launched = true;
}

void ntg_wait()
{
    if(!__launched) return;

    pthread_join(__ntg_thread, NULL);
}

void ntg_deinit()
{
    nt_cursor_show(NULL);
    nt_alt_screen_disable(NULL);
    nt_deinit();

   _ntg_log_deinit_();
}

static void* ntg_thread_fn(void* _data)
{
    struct thread_fn_data* data = (struct thread_fn_data*)_data;

    ntg_entity_system* es = _ntg_entity_system_new();

    ntg_loop* loop = ntg_loop_new(es);

    data->gui_fn(es, loop, data->gui_fn_data);

    _ntg_entity_system_destroy(es);

    free(data);

    return NULL;
}

/* TO MODIFY ntg_loop, CHANGE ALSO IN NTG_ENTITY_TYPE.C */
ntg_loop* ntg_loop_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_LOOP,
        .deinit_fn = NULL,
        .system = system
    };

    ntg_loop* new = (ntg_loop*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}
