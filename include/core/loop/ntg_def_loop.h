#ifndef _NTG_DEF_LOOP_H_
#define _NTG_DEF_LOOP_H_

#include <stdbool.h>
#include "core/loop/ntg_loop.h"

typedef struct ntg_def_loop ntg_def_loop;
typedef struct ntg_renderer ntg_renderer;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_delegate ntg_event_delegate;
struct nt_key_event;

typedef enum ntg_def_loop_process_key_mode
{
    NTG_DEF_LOOP_PROCESS_KEY_STAGE_FIRST,
    NTG_DEF_LOOP_PROCESS_KEY_LOOP_FIRST
} ntg_def_loop_process_key_mode;

typedef bool (*ntg_def_loop_process_key_fn)(
        ntg_loop_ctx* ctx,
        struct nt_key_event key);
typedef void (*ntg_def_loop_on_resize_fn)(
        ntg_loop_ctx* ctx,
        struct nt_resize_event resize);
typedef void (*ntg_def_loop_on_timeout_fn)(
        ntg_loop_ctx* ctx);

struct ntg_def_loop
{
    ntg_loop __base;

    unsigned int __framerate;
    unsigned int __it_elapsed;
    ntg_renderer* __renderer;

    ntg_def_loop_process_key_fn __process_key_fn;
    ntg_def_loop_process_key_mode __process_key_mode;
    ntg_def_loop_on_resize_fn __on_resize_fn;
    ntg_def_loop_on_timeout_fn __on_timeout_fn;
};

void __ntg_def_loop_init__(
        ntg_def_loop* loop,
        ntg_stage* init_stage,
        ntg_taskmaster* taskmaster,
        unsigned int framerate,
        ntg_renderer* renderer,
        ntg_def_loop_process_key_fn process_key_fn,
        ntg_def_loop_on_resize_fn on_resize_fn,
        ntg_def_loop_on_timeout_fn on_timeout_fn,
        void* data);
void __ntg_def_loop_deinit__(ntg_def_loop* loop);

void ntg_def_loop_set_process_key_mode(ntg_def_loop* loop,
        ntg_def_loop_process_key_mode mode);
ntg_def_loop_process_key_mode ntg_def_loop_get_process_key_mode(
        const ntg_def_loop* loop);

struct ntg_loop_status __ntg_def_loop_process_event_fn(
        ntg_loop* _loop,
        ntg_loop_ctx* ctx,
        struct nt_event event);

#endif // _NTG_DEF_LOOP_H_
