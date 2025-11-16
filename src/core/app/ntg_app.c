#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/app/ntg_app.h"
#include "base/event/ntg_event.h"
#include "base/event/ntg_event_participants.h"
#include "base/event/ntg_event_types.h"
#include "core/app/ntg_app_renderer.h"
#include "core/scene/ntg_scene.h"
#include "core/scene/shared/_ntg_drawing.h"
#include "core/stage/ntg_stage.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "pthread.h"
#include "nt.h"

#include "shared/ntg_log.h"
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */

static ntg_event_delegate* __del = NULL;

static pthread_t __ntg_thread;
static bool __launched = false;

static void* __ntg_app_thread_fn(void* _thread_fn_data);

/* -------------------------------------------------------------------------- */

void __ntg_app_init__()
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

    __del = ntg_event_delegate_new();
}

struct ntg_app_thread_fn_data
{
    ntg_app_gui_fn gui_fn;
    void* gui_fn_data;
};

void ntg_app_launch(ntg_app_gui_fn gui_fn, void* data)
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
            __ntg_app_thread_fn, thread_fn_data);

    assert(status == 0);

    __launched = true;
}

void* ntg_app_wait()
{
    if(!__launched) return NULL;

    void* _data;
    pthread_join(__ntg_thread, &_data);
    
    return _data;
}

void __ntg_app_deinit__()
{
    nt_cursor_show(NULL);
    nt_alt_screen_disable(NULL);
    __nt_deinit__();

   __ntg_log_deinit__();

   ntg_event_delegate_destroy(__del);
}

/* -------------------------------------------------------------------------- */

void ntg_app_loop(
        ntg_stage* init_stage,
        uint framerate,
        ntg_app_renderer* renderer,
        ntg_app_process_key_fn process_key_fn,
        void* data)
{
    assert(process_key_fn != NULL);
    assert(renderer != NULL);
    assert(init_stage != NULL);

    struct ntg_app_loop_context context = {
        .stage = init_stage
    };

    size_t _width, _height;
    nt_get_term_size(&_width, &_height);
    struct ntg_xy app_size = ntg_xy(_width, _height);
    struct ntg_xy old_app_size = ntg_xy(0, 0);

    ntg_stage_drawing* empty_drawing = ntg_stage_drawing_new();

    nt_status _status;
    struct nt_event event;
    uint timeout = NTG_APP_WAIT_TIMEOUT(framerate); 
    bool loop = true;
    while(loop)
    {
        event = nt_wait_for_event(timeout, &_status);

        timeout = NTG_APP_WAIT_TIMEOUT(framerate) - event.elapsed;

        size_t _width, _height;
        ntg_app_status status;
        const ntg_stage_drawing* drawing;
        ntg_event resize_ev;
        struct ntg_evt_app_resize_data resize_data;
        switch(event.type)
        {
            case NT_EVENT_TYPE_KEY:
                status = process_key_fn(event.key_data, &context, data);
                if(status == NTG_APP_QUIT)
                    loop = false;
                break;

            case NT_EVENT_TYPE_RESIZE:
                nt_get_term_size(&_width, &_height);
                old_app_size = app_size;
                app_size = ntg_xy(_width, _height);

                resize_data = (struct ntg_evt_app_resize_data) {
                    .old = old_app_size,
                    .new = app_size
                };

                __ntg_event_init__(
                        &resize_ev,
                        NTG_EVT_APP_RESIZE,
                        NTG_EVENT_PARTICIPANT_APP,
                        &resize_data);

                ntg_event_delegate_raise(__del, &resize_ev);

                break;

            case NT_EVENT_TYPE_TIMEOUT:
                if(context.stage != NULL)
                {
                    ntg_stage_compose(context.stage, app_size);
                    drawing = ntg_stage_get_drawing(context.stage);
                }
                else
                {
                    ntg_stage_drawing_set_size(empty_drawing, app_size);
                    drawing = empty_drawing;
                }

                ntg_app_renderer_render(renderer, drawing, app_size);
                break;
        }

        assert(_status == NT_SUCCESS);
    }

    ntg_stage_drawing_destroy(empty_drawing);
    empty_drawing = NULL;
}

static void* __ntg_app_thread_fn(void* _thread_fn_data)
{
    struct ntg_app_thread_fn_data* data =
        (struct ntg_app_thread_fn_data*)_thread_fn_data;

    if(data->gui_fn != NULL) data->gui_fn(data->gui_fn_data);

    free(data);

    return NULL;
}

ntg_listenable* ntg_app_get_listenable()
{
    return ntg_event_delegate_listenable(__del);
}
