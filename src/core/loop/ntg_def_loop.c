// #include <assert.h>
// #include <stdlib.h>
//
// #include "core/loop/ntg_def_loop.h"
// #include "core/loop/ntg_loop_context.h"
// #include "core/renderer/ntg_renderer.h"
// #include "core/stage/ntg_stage.h"
// #include "core/stage/shared/ntg_stage_drawing.h"
// #include "nt.h"
// #include "shared/ntg_xy.h"
//
// typedef struct ntg_def_loop_context
// {
//     ntg_loop_context __base;
//
//     /* read-only */
//     ntg_renderer* _renderer;
//     unsigned int _framerate;
//
//     unsigned int timeout;
//
//     ntg_def_loop_process_key_fn _process_key_fn;
//     ntg_def_loop_process_key_mode _process_key_mode;
//     ntg_def_loop_process_resize_fn _process_resize_fn;
//     ntg_def_loop_process_timeout_fn _process_timeout_fn;
// } ntg_def_loop_context;
//
// void __ntg_def_loop_init__(
//         ntg_def_loop* loop,
//         ntg_stage* init_stage,
//         unsigned int framerate,
//         ntg_renderer* renderer,
//         ntg_def_loop_process_key_fn process_key_fn,
//         ntg_def_loop_process_resize_fn process_resize_fn,
//         ntg_def_loop_process_timeout_fn process_timeout_fn,
//         void* data)
// {
//     assert(loop != NULL);
//     assert(init_stage != NULL);
//     assert((framerate > 0) && (framerate < 1000));
//     assert(renderer != NULL);
//
//     __ntg_loop_init__(
//             (ntg_loop*)loop,
//             __ntg_def_loop_create_context_fn,
//             __ntg_def_loop_destroy_context_fn,
//             __ntg_def_loop_loop_fn,
//             init_stage,
//             data);
//
//     loop->__framerate = framerate;
//     loop->__renderer = renderer;
//     loop->__process_key_fn = process_key_fn;
//     loop->__process_key_mode = NTG_DEF_LOOP_PROCESS_KEY_STAGE_FIRST;
//     loop->__process_resize_fn = process_resize_fn;
//     loop->__process_timeout_fn = process_timeout_fn;
// }
//
// void __ntg_def_loop_deinit__(ntg_def_loop* loop)
// {
//     assert(loop != NULL);
//
//     __ntg_loop_deinit__((ntg_loop*)loop);
//
//     loop->__framerate = 0;
//     loop->__renderer = NULL;
//     loop->__process_key_fn = NULL;
//     loop->__process_key_mode = NTG_DEF_LOOP_PROCESS_KEY_STAGE_FIRST;
//     loop->__process_resize_fn = NULL;
//     loop->__process_timeout_fn = NULL;
// }
//
// void ntg_def_loop_set_process_key_mode(ntg_def_loop* loop,
//         ntg_def_loop_process_key_mode mode)
// {
//     assert(loop != NULL);
//
//     loop->__process_key_mode = mode;
// }
// ntg_def_loop_process_key_mode ntg_def_loop_get_process_key_mode(
//         const ntg_def_loop* loop)
// {
//     assert(loop != NULL);
//
//     return loop->__process_key_mode;
// }
//
// struct ntg_loop_status __ntg_def_loop_loop_fn(ntg_loop_context* _ctx)
// {
//     assert(_ctx != NULL);
//
//     ntg_def_loop_context* ctx = (ntg_def_loop_context*)_ctx;
//
//     /* Retrieve from context */
//     ntg_renderer* renderer = ctx->_renderer;
//     unsigned int framerate = ctx->_framerate;
//     ntg_stage* curr_stage = ntg_loop_context_get_stage(_ctx);
//     struct ntg_xy app_size = ntg_loop_context_get_app_size(_ctx);
//
//     /* Wait for event */
//     nt_status _status;
//     struct nt_event event = nt_wait_for_event(ctx->timeout, &_status);
//
//     const ntg_stage_drawing* drawing;
//     switch(event.type)
//     {
//         case NT_EVENT_KEY:
//             ctx->timeout -= event.elapsed;
//
//             if(ctx->_process_key_mode == NTG_DEF_LOOP_PROCESS_KEY_STAGE_FIRST)
//             {
//                 bool consumed = false;
//                 if(curr_stage != NULL)
//                 {
//                     consumed = ntg_stage_feed_key_event(curr_stage,
//                             event.key_data);
//                 }
//                 if((!consumed) && (ctx->_process_key_fn != NULL))
//                     consumed = ctx->_process_key_fn(_ctx, event.key_data);
//             }
//             else
//             {
//                 bool consumed = false;
//                 if((ctx->_process_key_fn != NULL))
//                     consumed = ctx->_process_key_fn(_ctx, event.key_data);
//
//                 if((!consumed) && (curr_stage != NULL))
//                 {
//                     consumed = ntg_stage_feed_key_event(curr_stage,
//                             event.key_data);
//                 }
//             }
//             break;
//
//         case NT_EVENT_RESIZE:
//             ctx->timeout -= event.elapsed;
//             if(ctx->_process_resize_fn != NULL)
//                 ctx->_process_resize_fn(_ctx, event.resize_data);
//             break;
//
//         case NT_EVENT_TIMEOUT:
//             ctx->timeout = (1.0 * 1000) / framerate;
//             if(curr_stage != NULL)
//             {
//                 ntg_stage_compose(curr_stage, app_size);
//                 drawing = ntg_stage_get_drawing(curr_stage);
//             }
//             else drawing = NULL;
//
//             ntg_renderer_render(renderer, drawing, app_size);
//
//             if(ctx->_process_timeout_fn != NULL)
//                 ctx->_process_timeout_fn(_ctx);
//
//             break;
//     }
//
//     return (struct ntg_loop_status) {
//         .event = event
//     };
// }
//
// ntg_loop_context* __ntg_def_loop_create_context_fn(ntg_loop* _loop, void* context_data)
// {
//     ntg_def_loop* loop = (ntg_def_loop*)_loop;
//
//     ntg_def_loop_context* new = (ntg_def_loop_context*)malloc(
//             sizeof(ntg_def_loop_context));
//     assert(new != NULL);
//
//     struct ntg_xy app_size;
//     nt_get_term_size(&app_size.x, &app_size.y);
//
//     __ntg_loop_context_init__((ntg_loop_context*)new, _loop->__init_stage,
//             app_size, context_data);
//
//     new->_framerate = loop->__framerate;
//     new->_renderer = loop->__renderer;
//     new->timeout = (1.0 * 1000) / new->_framerate;
//
//     new->_process_key_fn = loop->__process_key_fn;
//     new->_process_key_mode = loop->__process_key_mode;
//     new->_process_resize_fn = loop->__process_resize_fn;
//     new->_process_timeout_fn = loop->__process_timeout_fn;
//
//     return (ntg_loop_context*)new;
// }
//
// void __ntg_def_loop_destroy_context_fn(ntg_loop_context* _context)
// {
//     __ntg_loop_context_deinit__(_context);
//
//     free(_context);
// }
