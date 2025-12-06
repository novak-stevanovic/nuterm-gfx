#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#define _NTG_TASKMASTER_PRIVATE_
#include "core/loop/ntg_taskmaster.h"
#undef _NTG_TASKMASTER_PRIVATE_

#include "core/loop/_ntg_callback_queue.h"

ntg_taskmaster* ntg_taskmaster_new()
{
    ntg_taskmaster* new = (ntg_taskmaster*)malloc(sizeof(ntg_taskmaster));
    assert(new != NULL);

    __ntg_taskmaster_init__(new);

    return new;
}

void ntg_taskmaster_destroy(ntg_taskmaster* taskmaster)
{
    assert(taskmaster != NULL);

    __ntg_taskmaster_deinit__(taskmaster);

    free(taskmaster);
}

void __ntg_taskmaster_init__(ntg_taskmaster* taskmaster)
{
    assert(taskmaster != NULL);

    pthread_mutex_init(&taskmaster->__lock, NULL);
    taskmaster->__callbacks = ntg_callback_queue_new();

    taskmaster->__channel.__taskmaster = taskmaster;
}

void __ntg_taskmaster_deinit__(ntg_taskmaster* taskmaster)
{
    assert(taskmaster != NULL);

    pthread_mutex_destroy(&taskmaster->__lock);
    ntg_callback_queue_destroy(taskmaster->__callbacks);
    taskmaster->__callbacks = NULL;
    taskmaster->__channel.__taskmaster = NULL;
}

struct ntg_thread_fn_data
{
    ntg_taskmaster* taskmaster;
    void* (*task_fn)(void* task_data);
    void* task_data;
    void (*callback_fn)(void* callback_data, void* task_result);
    void* callback_data;
};

struct ntg_callback_fn_data
{
    struct ntg_thread_fn_data base;
    void* task_result;
};

static void __callback_fn(void* _data)
{
    struct ntg_callback_fn_data* data = (struct ntg_callback_fn_data*)_data;

    if(data->base.callback_fn)
    {
        data->base.callback_fn(data->base.callback_data, data->task_result);
    }
    free(data);
}

static void* __thread_fn(void* _data)
{
    struct ntg_thread_fn_data* data = (struct ntg_thread_fn_data*)_data;

    void* task_result = data->task_fn(data->task_data);

    struct ntg_callback_fn_data* new_data = (struct ntg_callback_fn_data*)malloc(
            sizeof(struct ntg_callback_fn_data));
    assert(new_data != NULL);

    (*new_data) = (struct ntg_callback_fn_data) {
        .base = *data,
        .task_result = task_result
    };

    ntg_callback_queue_append(
            data->taskmaster->__callbacks,
            __callback_fn, new_data);

    free(data);

    return NULL;
}

void ntg_taskmaster_execute_task(
        ntg_taskmaster_channel* _taskmaster,
        void* (*task_fn)(void* task_data),
        void* task_data,
        void (*callback_fn)(void* callback_data, void* task_result),
        void* callback_data)
{
    assert(_taskmaster != NULL);

    ntg_taskmaster* taskmaster = _taskmaster->__taskmaster;

    if(task_fn == NULL) return;

    pthread_mutex_lock(&taskmaster->__lock);

    struct ntg_thread_fn_data* data = (struct ntg_thread_fn_data*)malloc(
            sizeof(struct ntg_thread_fn_data));
    assert(data != NULL);

    (*data) = (struct ntg_thread_fn_data) {
        .taskmaster = taskmaster,
        .task_fn = task_fn,
        .task_data = task_data,
        .callback_fn = callback_fn,
        .callback_data = callback_data,
    };

    pthread_t thread;
    int status = pthread_create(&thread, NULL, __thread_fn, data);
    assert(status == 0);
    pthread_detach(thread);

    pthread_mutex_unlock(&taskmaster->__lock);
}

ntg_taskmaster_channel* ntg_taskmaster_get_channel(ntg_taskmaster* taskmaster)
{
    assert(taskmaster != NULL);

    return &taskmaster->__channel;
}

void ntg_taskmaster_execute_callbacks(ntg_taskmaster* taskmaster)
{
    assert(taskmaster != NULL);

    ntg_callback_queue_run(taskmaster->__callbacks);
}
