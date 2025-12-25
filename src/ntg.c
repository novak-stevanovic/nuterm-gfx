#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "ntg.h"
#include "base/entity/_ntg_entity_system.h"
#include "nt.h"
#include "shared/ntg_log.h"
#include "shared/sarena.h"

static ntg_loop* ntg_loop_new(ntg_entity_system* system);
static void _ntg_loop_deinit_fn(ntg_entity* entity);

/* -------------------------------------------------------------------------- */

static pthread_t __ntg_thread;
static bool __launched = false;

static void* ntg_thread_fn(void* _thread_fn_data);

/* -------------------------------------------------------------------------- */

void _ntg_init_()
{
    _ntg_log_init_("ntg_log.txt");

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

void* ntg_wait()
{
    if(!__launched) return NULL;

    void* _data;
    pthread_join(__ntg_thread, &_data);
    
    return _data;
}

void _ntg_deinit_()
{
    nt_cursor_show(NULL);
    nt_alt_screen_disable(NULL);
    __nt_deinit__();

   _ntg_log_deinit_();
}

static void* ntg_thread_fn(void* _thread_fn_data)
{
    struct thread_fn_data* data =
        (struct thread_fn_data*)_thread_fn_data;

    ntg_entity_system* es = ntg_entity_system_new();

    ntg_loop* loop = ntg_loop_new(es);

    data->gui_fn(loop, es, data->gui_fn_data);

    ntg_entity_system_destroy(es);

    free(data);

    return NULL;
}

/* TO MODIFY, CHANGE ALSO IN _NTG_ENTITY_TYPE.C */
ntg_loop* ntg_loop_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_LOOP,
        .deinit_fn = _ntg_loop_deinit_fn,
        .system = system
    };

    ntg_loop* new = (ntg_loop*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_loop_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    _ntg_entity_deinit_fn(entity);
}

