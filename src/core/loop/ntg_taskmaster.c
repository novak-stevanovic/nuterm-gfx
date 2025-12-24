#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "core/loop/ntg_taskmaster.h"

#include "base/entity/ntg_entity.h"
#include "core/loop/_ntg_callback_queue.h"
#include <pthread.h>

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

ntg_taskmaster* ntg_taskmaster_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_TASKMASTER,
        .deinit_fn = _ntg_taskmaster_deinit_fn,
        .system = system
    };

    ntg_taskmaster* new = (ntg_taskmaster*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_taskmaster_init_(ntg_taskmaster* taskmaster)
{
    assert(taskmaster != NULL);

    taskmaster->data = malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(taskmaster->data, NULL);
    taskmaster->__callbacks = ntg_callback_queue_new();
}

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

void _ntg_taskmaster_deinit_fn(ntg_entity* entity)
{
    assert(entity != NULL);

    ntg_taskmaster* taskmaster = (ntg_taskmaster*)entity;

    pthread_mutex_destroy((pthread_mutex_t*)taskmaster->data);
    ntg_callback_queue_destroy(taskmaster->__callbacks);
    taskmaster->__callbacks = NULL;

    free(taskmaster->data);

    _ntg_entity_deinit_fn(entity);
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

static void callback_fn(void* _data)
{
    struct ntg_callback_fn_data* data = (struct ntg_callback_fn_data*)_data;

    if(data->base.callback_fn)
    {
        data->base.callback_fn(data->base.callback_data, data->task_result);
    }
    free(data);
}

static void* thread_fn(void* _data)
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
            callback_fn, new_data);

    free(data);

    return NULL;
}

void ntg_taskmaster_execute_task(
        ntg_taskmaster* taskmaster,
        void* (*task_fn)(void* task_data),
        void* task_data,
        void (*callback_fn)(void* callback_data, void* task_result),
        void* callback_data)
{
    assert(taskmaster != NULL);

    if(task_fn == NULL) return;

    pthread_mutex_lock((pthread_mutex_t*)taskmaster->data);

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
    int status = pthread_create(&thread, NULL, thread_fn, data);
    assert(status == 0);
    pthread_detach(thread);

    pthread_mutex_unlock((pthread_mutex_t*)taskmaster->data);
}

void ntg_taskmaster_execute_callbacks(ntg_taskmaster* taskmaster)
{
    assert(taskmaster != NULL);

    ntg_callback_queue_run(taskmaster->__callbacks);
}

