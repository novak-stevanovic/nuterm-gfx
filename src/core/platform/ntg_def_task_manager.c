#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "core/platform/ntg_def_task_manager.h"
#include "core/platform/ntg_platform.h"
#include "shared/ntg_lock.h"
#include "shared/sarena.h"

void __ntg_def_task_manager_init__(
        ntg_def_task_manager* task_manager,
        ntg_platform_channel* platform_channel)
{
    assert(task_manager != NULL);

    __ntg_task_manager_init__(
            (ntg_task_manager*)task_manager,
            __ntg_def_task_manager_execute_fn,
            platform_channel);

    sa_err _err;
    task_manager->__arena = sarena_create(10000, &_err);
    assert(_err == SA_SUCCESS);
}

// TODO: what if tasks are active?
void __ntg_def_task_manager_deinit__(ntg_def_task_manager* task_manager)
{
    assert(task_manager != NULL);

    __ntg_task_manager_deinit__((ntg_task_manager*)task_manager);

    sarena_destroy(task_manager->__arena);
    task_manager->__arena = NULL;
}

struct thread_data
{
    ntg_def_task_manager* manager;
    bool (*task_fn)(void* task_data);
    void* task_data_cpy;
    void (*callback_fn)(void* callback_data);
    void* callback_data_cpy;
    size_t callback_data_size;
};

static void* __thread_fn(void* _data)
{
    struct thread_data* data = (struct thread_data*)_data;

    ntg_task_manager* _manager = (ntg_task_manager*)data->manager;

    ntg_lock_lock(_manager->_lock); // ----------------------------------------

    (data->manager->__active_tasks)++;

    ntg_lock_unlock(_manager->_lock); // --------------------------------------

    bool callback_status = data->task_fn(data->task_data_cpy);

    if(callback_status && (data->callback_fn != NULL))
    {
        struct ntg_platform_request req = {
            .action_fn = data->callback_fn,
            .data = data->callback_data_cpy
        };

        ntg_platform_channel_execute_later(
                _manager->_platform_channel,
                req,
                data->callback_data_size);
    }

    ntg_lock_lock(_manager->_lock); // ----------------------------------------

    (data->manager->__active_tasks)--;

    if(data->manager->__active_tasks == 0)
        sarena_rewind(data->manager->__arena);

    ntg_lock_unlock(_manager->_lock); // --------------------------------------

    free(data->task_data_cpy);
    free(data->callback_data_cpy);
    free(data);

    return NULL;
}

void __ntg_def_task_manager_execute_fn(
        ntg_task_manager* _manager,
        bool (*task_fn)(void* task_data),
        void* task_data,
        size_t task_data_size,
        void (*callback_fn)(void* callback_data),
        void* callback_data,
        size_t callback_data_size)
{
    struct thread_data* data = (struct thread_data*)malloc(
            sizeof(struct thread_data));
    assert(data != NULL);

    ntg_def_task_manager* manager = (ntg_def_task_manager*)_manager;

    sa_err _err;
    void* task_data_cpy = NULL;
    void* callback_data_cpy = NULL;
    if(task_data != NULL)
    {
        task_data_cpy = sarena_malloc(manager->__arena, task_data_size, &_err);
        memmove(task_data_cpy, task_data, task_data_size);
        assert(_err == SA_SUCCESS);
    }
    if(callback_data != NULL)
    {
        callback_data_cpy = sarena_malloc(manager->__arena, callback_data_size, &_err);
        assert(_err == SA_SUCCESS);
        memmove(callback_data_cpy, callback_data, callback_data_size);
    }

    (*data) = (struct thread_data) {
        .manager = manager,
        .task_fn = task_fn,
        .task_data_cpy = task_data_cpy,
        .callback_fn = callback_fn,
        .callback_data_cpy = callback_data_cpy,
        .callback_data_size = callback_data_size
    };

    pthread_t thread;
    int status = pthread_create(&thread, NULL, __thread_fn, data);
    assert(status == 0);

    pthread_detach(thread);
}
