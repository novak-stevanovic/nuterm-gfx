#ifndef _NTG_DEF_LOOP_PROVIDER_H_
#define _NTG_DEF_LOOP_PROVIDER_H_

#include <stdbool.h>
#include "core/loop_provider/ntg_loop_provider.h"

typedef struct ntg_def_loop_provider ntg_def_loop_provider;
typedef struct ntg_renderer ntg_renderer;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_delegate ntg_event_delegate;
struct nt_key_event;

/* Returns if the key had been consumed */
typedef bool (*ntg_dlp_process_key_fn)(
        ntg_def_loop_provider* loop_provider,
        struct nt_key_event key,
        void* data);

struct ntg_def_loop_provider
{
    ntg_loop_provider __base;

    unsigned int __framerate;
    ntg_renderer* __renderer;
    ntg_dlp_process_key_fn __process_key_fn;
    void* __process_key_data;

    ntg_event_delegate* __delegate;
};

void __ntg_def_loop_provider_init__(
        ntg_def_loop_provider* loop_provider,
        ntg_stage* init_stage,
        unsigned int framerate,
        ntg_renderer* renderer,
        ntg_dlp_process_key_fn process_key_fn,
        void* process_key_fn_data);
void __ntg_def_loop_provider_deinit__(ntg_def_loop_provider* loop_provider);

/* Raises NTG_EVT_APP_RESIZE */
ntg_listenable* ntg_def_loop_provider_get_listenable(
        ntg_def_loop_provider* loop_provider);

void __ntg_def_loop_provider_run_fn(ntg_loop_provider* _loop_provider);

#endif // _NTG_DEF_LOOP_PROVIDER_H_
