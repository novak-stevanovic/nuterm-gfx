#ifndef _NTG_PLATFORM_H_
#define _NTG_PLATFORM_H_

#include <stddef.h>

struct ntg_platform_request
{
    void* data;
    void (*action_fn)(void* data);
};

typedef struct ntg_platform ntg_platform;
/* Opaque type used to add platform requests to queue */
typedef struct ntg_platform_channel ntg_platform_channel;

/* -------------------------------------------------------------------------- */

ntg_platform* ntg_platform_new();
void ntg_platform_destroy(ntg_platform* platform);

void ntg_platform_run(ntg_platform* platform);

ntg_platform_channel* ntg_platform_get_channel(ntg_platform* platform);

/* -------------------------------------------------------------------------- */

/* Copies requests's data. */
void ntg_platform_channel_execute_later(
        ntg_platform_channel* channel,
        struct ntg_platform_request request,
        size_t request_data_size);

#endif // _NTG_PLATFORM_H_
