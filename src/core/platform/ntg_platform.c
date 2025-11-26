#include <assert.h>
#include <stdlib.h>

#include "core/platform/ntg_platform.h"
#include "shared/ntg_lock.h"

void __ntg_platform_init__(
        ntg_platform* platform,
        ntg_platform_run_fn run_fn,
        ntg_platform_append_req_fn append_req_fn,
        void* data)
{
    assert(platform != NULL);
    assert(run_fn != NULL);
    assert(append_req_fn != NULL);

    platform->__lock = ntg_lock_new();
    platform->__run_fn = run_fn;
    platform->__append_req_fn = append_req_fn;
    platform->__data = data;
}

void __ntg_platform_deinit__(ntg_platform* platform)
{
    assert(platform != NULL);

    ntg_lock_destroy(platform->__lock);
    platform->__lock = NULL;
    platform->__run_fn = NULL;
    platform->__append_req_fn = NULL;
    platform->__data = NULL;
}

void ntg_platform_run(ntg_platform* platform)
{
    assert(platform != NULL);

    ntg_lock_lock(platform->__lock);
    platform->__run_fn(platform);
    ntg_lock_unlock(platform->__lock);
}

ntg_platform_channel ntg_platform_get_channel(ntg_platform* platform)
{
    assert(platform != NULL);

    return (ntg_platform_channel) {
        .__platform = platform
    };
}

void ntg_platform_execute_later(
        ntg_platform_channel* channel,
        struct ntg_platform_request request)
{
    assert(channel != NULL);

    ntg_lock_lock(channel->__platform->__lock);

    ntg_platform* platform = channel->__platform;
    platform->__append_req_fn(platform, request);

    ntg_lock_unlock(channel->__platform->__lock);
}
