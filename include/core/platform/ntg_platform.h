#ifndef _NTG_PLATFORM_H_
#define _NTG_PLATFORM_H_

typedef struct ntg_platform ntg_platform;
typedef struct ntg_lock ntg_lock;

struct ntg_platform_request
{
    void* data; // will be freed after executing finishes
    void (*action_fn)(void* data);
};

/* Opaque type used to add platform requests to queue */
typedef struct ntg_platform_channel ntg_platform_channel;

/* -------------------------------------------------------------------------- */

/* Executes all current requests in order. Must also free the memory allocated
 * for each request's data. */
typedef void (*ntg_platform_run_fn)(ntg_platform* platform);

/* Appends request to queue. All of the requests will be executed sequentially
 * when ntg_platform_run() is called */
typedef void (*ntg_platform_append_req_fn)(
        ntg_platform* platform,
        struct ntg_platform_request request);

struct ntg_platform
{
    ntg_lock* __lock;
    ntg_platform_run_fn __run_fn;
    ntg_platform_append_req_fn __append_req_fn;

    void* __data;
};

void __ntg_platform_init__(
        ntg_platform* platform,
        ntg_platform_run_fn run_fn,
        ntg_platform_append_req_fn append_req_fn,
        void* data);
void __ntg_platform_deinit__(ntg_platform* platform);

void ntg_platform_run(ntg_platform* platform);

ntg_platform_channel ntg_platform_get_channel(ntg_platform* platform);

void ntg_platform_execute_later(
        ntg_platform_channel* channel,
        struct ntg_platform_request request);

/* -------------------------------------------------------------------------- */
/* PRIVATE */
/* -------------------------------------------------------------------------- */

typedef struct ntg_platform_channel
{
    ntg_platform* __platform;
} ntg_platform_channel;

#endif // _NTG_PLATFORM_H_
