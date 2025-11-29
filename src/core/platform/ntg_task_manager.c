#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "core/platform/ntg_task_manager.h"
#include "core/platform/ntg_callback_queue.h"
#include "core/platform/ntg_callback_queue.h"
#include "shared/ntg_lock.h"

void __ntg_task_manager_init__(
        ntg_task_manager* task_manager,
        ntg_callback_queue_channel* channel)
{
    assert(task_manager != NULL);

    task_manager->__channel = channel;
    task_manager->__lock = ntg_lock_new();
}

void __ntg_task_manager_deinit__(ntg_task_manager* task_manager)
{
    assert(task_manager != NULL);

    task_manager->__channel = NULL;
    ntg_lock_destroy(task_manager->__lock);
    task_manager->__lock = NULL;
}

struct ntg_thread_fn_data
{
    ntg_task_manager* task_manager;
    void* (*task_fn)(void* task_data);
    void* task_data;
    void (*callback_fn)(void* callback_data, void* task_result);
    void* callback_data;
};

struct ntg_callback_fn_data
{
    struct ntg_thread_fn_data* base;
    void* task_result;
};

static void __callback_fn(void* _data)
{
    struct ntg_callback_fn_data* data = (struct ntg_callback_fn_data*)_data;

    if(data->base->callback_fn)
    {
        data->base->callback_fn(data->base->callback_data, data->task_result);
    }

    /* Clean-up */
    if(data->base->task_data != NULL)
        free(data->base->task_data);
    if(data->base->callback_data != NULL)
        free(data->base->callback_data);
    free(data->base);
    if(data->task_result != NULL)
        free(data->task_result);
}

static void* __thread_fn(void* _data)
{
    struct ntg_thread_fn_data* data = (struct ntg_thread_fn_data*)_data;

    void* task_result = data->task_fn(data->task_data);

    struct ntg_callback_fn_data* new_data = (struct ntg_callback_fn_data*)malloc(
            sizeof(struct ntg_callback_fn_data));
    assert(new_data != NULL);

    (*new_data) = (struct ntg_callback_fn_data) {
        .base = data,
        .task_result = task_result
    };

    if(data->callback_fn)
    {
        ntg_callback_queue_channel* channel = data->task_manager->__channel;
        ntg_callback_queue_channel_append(channel, __callback_fn, &new_data, 0);
    }

    return NULL;
}

void ntg_task_manager_execute_task(
        ntg_task_manager* task_manager,
        void* (*task_fn)(void* task_data),
        void* task_data,
        void (*callback_fn)(void* callback_data, void* task_result),
        void* callback_data)
{
    assert(task_manager != NULL);

    if(task_fn == NULL) return;

    ntg_lock_lock(task_manager->__lock);

    struct ntg_thread_fn_data* data = malloc(sizeof(struct ntg_thread_fn_data));
    assert(data != NULL);

    (*data) = (struct ntg_thread_fn_data) {
        .task_manager = task_manager,
        .task_fn = task_fn,
        .task_data = task_data,
        .callback_fn = callback_fn,
        .callback_data = callback_data
    };

    pthread_t thread;
    pthread_create(&thread, NULL, __thread_fn, &data);
    pthread_detach(thread);

    ntg_lock_unlock(task_manager->__lock);
}
