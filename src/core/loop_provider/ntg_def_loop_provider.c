#include <assert.h>
#include <stdlib.h>

#include "core/loop_provider/ntg_def_loop_provider.h"
#include "base/event/ntg_event.h"
#include "base/event/ntg_event_participants.h"
#include "base/event/ntg_event_types.h"
#include "core/renderer/ntg_renderer.h"
#include "core/stage/ntg_stage.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "nt.h"
#include "shared/ntg_xy.h"

void __ntg_def_loop_provider_init__(
        ntg_def_loop_provider* loop_provider,
        ntg_stage* init_stage,
        unsigned int framerate,
        ntg_renderer* renderer,
        ntg_dlp_process_key_fn process_key_fn,
        void* process_key_fn_data)
{
    assert(loop_provider != NULL);
    assert(init_stage != NULL);
    assert((framerate > 0) && (framerate < 1000));
    assert(renderer != NULL);
    assert(process_key_fn != NULL);

    __ntg_loop_provider_init__(
            (ntg_loop_provider*)loop_provider,
            __ntg_def_loop_provider_run_fn,
            init_stage);

    loop_provider->__framerate = framerate;
    loop_provider->__renderer = renderer;
    loop_provider->__process_key_fn = process_key_fn;
    loop_provider->__process_key_data = process_key_fn_data;

    loop_provider->__delegate = ntg_event_delegate_new();
}

void __ntg_def_loop_provider_deinit__(ntg_def_loop_provider* loop_provider)
{
    assert(loop_provider != NULL);

    __ntg_loop_provider_deinit__((ntg_loop_provider*)loop_provider);

    loop_provider->__framerate = 0;
    loop_provider->__renderer = NULL;
    loop_provider->__process_key_fn = NULL;
    loop_provider->__process_key_data = NULL;

    ntg_event_delegate_destroy(loop_provider->__delegate);
    loop_provider->__delegate = NULL;
}

/* Raises NTG_EVT_APP_RESIZE */
ntg_listenable* ntg_def_loop_provider_get_listenable(
        ntg_def_loop_provider* loop_provider)
{
    assert(loop_provider != NULL);

    return ntg_event_delegate_listenable(loop_provider->__delegate);
}

void __ntg_def_loop_provider_run_fn(ntg_loop_provider* _loop_provider)
{
    assert(_loop_provider != NULL);
    ntg_def_loop_provider* loop_provider = (ntg_def_loop_provider*)_loop_provider;

    ntg_renderer* renderer = loop_provider->__renderer;
    unsigned int framerate = loop_provider->__framerate;
    unsigned int timeout = 1.0 * 1000 / framerate;
    ntg_dlp_process_key_fn process_key_fn = loop_provider->__process_key_fn;
    void* data = loop_provider->__process_key_data;
    ntg_event_delegate* delegate = loop_provider->__delegate;

    size_t _width, _height;
    nt_get_term_size(&_width, &_height);
    struct ntg_xy app_size = ntg_xy(_width, _height);
    struct ntg_xy old_app_size = ntg_xy(0, 0);

    ntg_stage_drawing* empty_drawing = ntg_stage_drawing_new();

    nt_status _status;
    struct nt_event event;
    ntg_stage* curr_stage;
    while(_loop_provider->__loop)
    {
        _loop_provider->__stage = _loop_provider->__next_stage;
        curr_stage = _loop_provider->__stage;

        event = nt_wait_for_event(timeout, &_status);

        timeout = timeout - event.elapsed;

        size_t _width, _height;
        const ntg_stage_drawing* drawing;
        ntg_event resize_ev;
        struct ntg_evt_app_resize_data resize_data;
        switch(event.type)
        {
            case NT_EVENT_KEY:
                process_key_fn(loop_provider, event.key_data, data);

            case NT_EVENT_RESIZE:
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

                ntg_event_delegate_raise(delegate, &resize_ev);

                break;

            case NT_EVENT_TIMEOUT:
                if(curr_stage != NULL)
                {
                    ntg_stage_compose(curr_stage, app_size);
                    drawing = ntg_stage_get_drawing(curr_stage);
                }
                else
                {
                    ntg_stage_drawing_set_size(empty_drawing, app_size);
                    drawing = empty_drawing;
                }

                ntg_renderer_render(renderer, drawing, app_size);
                break;
        }

        assert(_status == NT_SUCCESS);
    }

    ntg_stage_drawing_destroy(empty_drawing);
    empty_drawing = NULL;
}
