#include <assert.h>
#include <stdlib.h>

#include "core/loop/ntg_def_loop.h"
#include "core/renderer/ntg_renderer.h"
#include "core/stage/ntg_stage.h"
#include "core/stage/shared/ntg_stage_drawing.h"
#include "nt.h"
#include "shared/ntg_xy.h"

void __ntg_def_loop_init__(
        ntg_def_loop* loop,
        unsigned int framerate,
        ntg_renderer* renderer,
        ntg_def_loop_process_key_fn process_key_fn,
        ntg_def_loop_on_resize_fn on_resize_fn,
        ntg_def_loop_on_timeout_fn on_timeout_fn,
        void* data)
{
    assert(loop != NULL);
    assert((framerate > 0) && (framerate < 1000));
    assert(renderer != NULL);

    __ntg_loop_init__(
            (ntg_loop*)loop,
            __ntg_def_loop_process_event_fn,
            data);

    loop->__framerate = framerate;
    loop->__it_elapsed = 0;
    loop->__renderer = renderer;
    loop->__process_key_fn = process_key_fn;
    loop->__process_key_mode = NTG_DEF_LOOP_PROCESS_KEY_STAGE_FIRST;
    loop->__on_resize_fn = on_resize_fn;
    loop->__on_timeout_fn = on_timeout_fn;
}

void __ntg_def_loop_deinit__(ntg_def_loop* loop)
{
    assert(loop != NULL);

    __ntg_loop_deinit__((ntg_loop*)loop);

    loop->__framerate = 0;
    loop->__renderer = NULL;
    loop->__process_key_fn = NULL;
    loop->__process_key_mode = NTG_DEF_LOOP_PROCESS_KEY_STAGE_FIRST;
    loop->__on_resize_fn = NULL;
    loop->__on_timeout_fn = NULL;
}

void ntg_def_loop_set_process_key_mode(ntg_def_loop* loop,
        ntg_def_loop_process_key_mode mode)
{
    assert(loop != NULL);

    loop->__process_key_mode = mode;
}
ntg_def_loop_process_key_mode ntg_def_loop_get_process_key_mode(
        const ntg_def_loop* loop)
{
    assert(loop != NULL);

    return loop->__process_key_mode;
}

struct ntg_loop_status __ntg_def_loop_process_event_fn(
        ntg_loop* _loop,
        ntg_loop_context* context,
        struct nt_event event)
{
    assert(_loop != NULL);

    ntg_def_loop* loop = (ntg_def_loop*)_loop;
    /* Retrieve from context */
    ntg_renderer* renderer = loop->__renderer;
    unsigned int framerate = loop->__framerate;
    ntg_stage* curr_stage = ntg_loop_context_get_stage(context);
    struct ntg_xy app_size = ntg_loop_context_get_app_size(context);

    const ntg_stage_drawing* drawing;
    unsigned int timeout;
    switch(event.type)
    {
        case NT_EVENT_KEY:
            loop->__it_elapsed += event.elapsed;
            timeout = ((1.0 * 1000) / framerate) - loop->__it_elapsed;

            if(loop->__process_key_mode == NTG_DEF_LOOP_PROCESS_KEY_STAGE_FIRST)
            {
                bool consumed = false;
                if(curr_stage != NULL)
                {
                    consumed = ntg_stage_feed_key_event(curr_stage,
                            event.key_data);
                }
                if((!consumed) && (loop->__process_key_fn != NULL))
                    consumed = loop->__process_key_fn(context, event.key_data);
            }
            else
            {
                bool consumed = false;
                if((loop->__process_key_fn != NULL))
                    consumed = loop->__process_key_fn(context, event.key_data);

                if((!consumed) && (curr_stage != NULL))
                {
                    consumed = ntg_stage_feed_key_event(curr_stage,
                            event.key_data);
                }
            }
            break;

        case NT_EVENT_RESIZE:
            loop->__it_elapsed += event.elapsed;
            timeout = ((1.0 * 1000) / framerate) - loop->__it_elapsed;

            if(loop->__on_resize_fn != NULL)
                loop->__on_resize_fn(context, event.resize_data);
            break;

        case NT_EVENT_TIMEOUT:
            loop->__it_elapsed = 0;
            timeout = ((1.0 * 1000) / framerate);

            if(curr_stage != NULL)
            {
                ntg_stage_compose(curr_stage, app_size);
                drawing = ntg_stage_get_drawing(curr_stage);
            }
            else drawing = NULL;

            ntg_renderer_render(renderer, drawing, app_size);

            if(loop->__on_timeout_fn != NULL)
                loop->__on_timeout_fn(context);

            break;
    }

    return (struct ntg_loop_status) {
        .timeout = timeout
    };
}
