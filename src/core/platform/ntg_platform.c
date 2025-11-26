#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "core/platform/ntg_platform.h"
#include "shared/ntg_log.h"
#include "shared/sarena.h"

struct ntg_platform_channel
{
    ntg_platform* platform;
};

struct ntg_platform_req_list_node
{
    struct ntg_platform_request req;
    struct ntg_platform_req_list_node* next;
};

struct ntg_platform_req_list
{
    struct ntg_platform_req_list_node *head, *tail;
};

struct ntg_platform
{
    pthread_mutex_t lock;
    sarena* arena;

    struct ntg_platform_req_list list;

    ntg_platform_channel _channel;
};

/* -------------------------------------------------------------------------- */

ntg_platform* ntg_platform_new()
{
    ntg_platform* new = (ntg_platform*)malloc(sizeof(ntg_platform));
    assert(new != NULL);

    pthread_mutex_init(&new->lock, NULL);
    new->list = (struct ntg_platform_req_list) { .head = NULL, .tail = NULL };
    new->_channel = (ntg_platform_channel) { .platform = new };

    sa_err _err;
    new->arena = sarena_create(10000, &_err);
    assert(_err == SA_SUCCESS);

    return new;
}

void ntg_platform_destroy(ntg_platform* platform)
{
    assert(platform != NULL);

    pthread_mutex_destroy(&platform->lock);
    platform->list = (struct ntg_platform_req_list) { .head = NULL, .tail = NULL };
    sarena_destroy(platform->arena);
    platform->arena = NULL;

    free(platform);
}

void ntg_platform_run(ntg_platform* platform)
{
    assert(platform != NULL);

    pthread_mutex_lock(&platform->lock);

    ntg_log_log("Platform: run");

    struct ntg_platform_req_list_node* it = platform->list.head;
    while(it != NULL)
    {
        ntg_log_log("Platform: exec");
        if(it->req.action_fn != NULL) 
            it->req.action_fn(it->req.data);
        it = it->next;
    }

    sarena_rewind(platform->arena);

    pthread_mutex_unlock(&platform->lock);
}

ntg_platform_channel* ntg_platform_get_channel(ntg_platform* platform)
{
    assert(platform != NULL);

    return &(platform->_channel);
}

/* -------------------------------------------------------------------------- */

void ntg_platform_channel_execute_later(
        ntg_platform_channel* channel,
        struct ntg_platform_request request,
        size_t request_data_size)
{
    assert(channel != NULL);
    assert(channel->platform != NULL);
    if(request.action_fn == NULL) return;

    ntg_platform* platform = channel->platform;

    pthread_mutex_lock(&platform->lock);

    sa_err _err;
    void* data_cpy = NULL;
    if(request.data != NULL)
    {
        data_cpy = sarena_malloc(platform->arena, request_data_size, &_err);
        assert(_err == SA_SUCCESS);

        memcpy(data_cpy, request.data, request_data_size);
    }
    
    struct ntg_platform_req_list_node* node = sarena_malloc(
            platform->arena,
            sizeof(struct ntg_platform_req_list_node),
            &_err);
    assert(_err == SA_SUCCESS);

    node->req = (struct ntg_platform_request) {
        .action_fn = request.action_fn,
        .data = data_cpy
    };
    node->next = NULL;

    if(platform->list.head == NULL)
    {
        platform->list.head = node;
        platform->list.tail = node;
    }
    else
    {
        platform->list.tail->next = node;
        platform->list.tail = node;
    }

    pthread_mutex_unlock(&platform->lock);
}
