#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "core/platform/ntg_callback_queue.h"
#include "shared/ntg_log.h"

struct ntg_callback_queue_channel
{
    ntg_callback_queue* callback_queue;
};

struct ntg_callback_queue_req
{
    void (*action_fn)(void* data);
    void* data;
};

struct ntg_callback_queue_req_list_node
{
    struct ntg_callback_queue_req req;
    struct ntg_callback_queue_req_list_node* next;
};

struct ntg_callback_queue_req_list
{
    struct ntg_callback_queue_req_list_node *head, *tail;
};

struct ntg_callback_queue
{
    pthread_mutex_t lock;

    struct ntg_callback_queue_req_list list;

    ntg_callback_queue_channel _channel;
};

/* -------------------------------------------------------------------------- */

ntg_callback_queue* ntg_callback_queue_new()
{
    ntg_callback_queue* new = (ntg_callback_queue*)malloc(sizeof(ntg_callback_queue));
    assert(new != NULL);

    pthread_mutex_init(&new->lock, NULL);
    new->list = (struct ntg_callback_queue_req_list) { .head = NULL, .tail = NULL };
    new->_channel = (ntg_callback_queue_channel) { .callback_queue = new };

    return new;
}

void ntg_callback_queue_destroy(ntg_callback_queue* callback_queue)
{
    assert(callback_queue != NULL);

    pthread_mutex_destroy(&callback_queue->lock);
    callback_queue->list = (struct ntg_callback_queue_req_list) { .head = NULL, .tail = NULL };

    free(callback_queue);
}

void ntg_callback_queue_run(ntg_callback_queue* callback_queue)
{
    assert(callback_queue != NULL);

    pthread_mutex_lock(&callback_queue->lock);

    ntg_log_log("Platform: run");

    struct ntg_callback_queue_req_list_node* it = callback_queue->list.head;
    while(it != NULL)
    {
        ntg_log_log("Platform: exec");
        if(it->req.action_fn != NULL) 
            it->req.action_fn(it->req.data);
        it = it->next;
    }

    pthread_mutex_unlock(&callback_queue->lock);
}

ntg_callback_queue_channel* ntg_callback_queue_get_channel(ntg_callback_queue* callback_queue)
{
    assert(callback_queue != NULL);

    return &(callback_queue->_channel);
}

/* -------------------------------------------------------------------------- */

void ntg_callback_queue_channel_append(
        ntg_callback_queue_channel* channel,
        void (*action_fn)(void* data),
        void* data,
        size_t request_data_size)
{
    assert(channel != NULL);
    assert(channel->callback_queue != NULL);
    if(action_fn == NULL) return;

    ntg_callback_queue* callback_queue = channel->callback_queue;

    pthread_mutex_lock(&callback_queue->lock);

    struct ntg_callback_queue_req_list_node* node = malloc(
            sizeof(struct ntg_callback_queue_req_list_node));
    assert(node != NULL);

    node->req = (struct ntg_callback_queue_req) {
        .action_fn = action_fn,
        .data = data
    };
    node->next = NULL;

    if(callback_queue->list.head == NULL)
    {
        callback_queue->list.head = node;
        callback_queue->list.tail = node;
    }
    else
    {
        callback_queue->list.tail->next = node;
        callback_queue->list.tail = node;
    }

    pthread_mutex_unlock(&callback_queue->lock);
}
