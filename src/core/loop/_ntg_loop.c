#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "core/loop/_ntg_loop.h"
#include "core/entity/ntg_entity_type.h"
#include "core/entity/ntg_entity.h"
#include "core/entity/ntg_entity_type.h"

/* -------------------------------------------------------------------------- */
/* LOOP */
/* -------------------------------------------------------------------------- */

ntg_loop* _ntg_loop_new(ntg_entity_system* system)
{
    struct ntg_entity_init_data entity_data = {
        .type = &NTG_ENTITY_LOOP,
        .deinit_fn = _ntg_loop_deinit_fn,
        .system = system
    };

    ntg_loop* new = (ntg_loop*)ntg_entity_create(entity_data);
    assert(new != NULL);

    return new;
}

void _ntg_loop_init(ntg_loop* loop)
{
    assert(loop != NULL);
}

void _ntg_loop_deinit_fn(ntg_entity* entity) {}

/* -------------------------------------------------------------------------- */
/* PLATFORM */
/* -------------------------------------------------------------------------- */

void _ntg_platform_init(ntg_platform* platform)
{
    assert(platform != NULL);

    pthread_mutex_init(&platform->__lock, NULL);

    ntg_ptask_list_init(&platform->__tasks, NULL);

    platform->__invalid = false;
}

void _ntg_platform_deinit(ntg_platform* platform)
{
    assert(platform != NULL);

    pthread_mutex_destroy(&platform->__lock);

    ntg_ptask_list_deinit(&platform->__tasks, NULL);

    platform->__invalid = true;
}

void _ntg_platform_execute_later(ntg_platform* platform, struct ntg_ptask task)
{
    assert(platform != NULL);

    pthread_mutex_lock(&platform->__lock);

    if(platform->__invalid)
    {
        pthread_mutex_unlock(&platform->__lock);
        return;
    }

    ntg_ptask_list_pushb(&platform->__tasks, task, NULL);

    pthread_mutex_unlock(&platform->__lock);
}

void _ntg_platform_execute_all(ntg_platform* platform, ntg_loop_ctx* loop_ctx)
{
    assert(platform != NULL);
    assert(loop_ctx != NULL);

    pthread_mutex_lock(&platform->__lock);

    if(platform->__invalid)
    {
        pthread_mutex_unlock(&platform->__lock);
        return;
    }

    struct ntg_ptask_list_node* it_node = platform->__tasks.head;

    while(it_node != NULL)
    {
        it_node->data->task_fn(it_node->data->data, loop_ctx);
        it_node = it_node->next;

        ntg_ptask_list_popf(&platform->__tasks, NULL);
    }

    pthread_mutex_unlock(&platform->__lock);
}

void _ntg_platform_invalidate(ntg_platform* platform)
{
    assert(platform != NULL);

    pthread_mutex_lock(&platform->__lock);
    platform->__invalid = true;
    pthread_mutex_unlock(&platform->__lock);
}

/* -------------------------------------------------------------------------- */
/* TASK RUNNER */
/* -------------------------------------------------------------------------- */

static void* worker_fn(void* _data);

void _ntg_task_runner_init(ntg_task_runner* task_runner,
        ntg_platform* platform, unsigned int worker_threads)
{
    assert(task_runner != NULL);
    assert(platform != NULL);
    assert(worker_threads < NTG_LOOP_WORKER_THREADS_MAX);

    task_runner->__platform = platform;
    task_runner->__thread_count = worker_threads;

    pthread_mutex_init(&task_runner->__lock, NULL);
    pthread_cond_init(&task_runner->__cond, NULL);

    task_runner->__init = true;

    ntg_task_list_init(&task_runner->__tasks, NULL);

    task_runner->__running = 0;

    int status;
    size_t i;
    for(i = 0; i < worker_threads; i++)
    {
        status = pthread_create(&(task_runner->__threads[i]), NULL,
            worker_fn, task_runner);

        assert(status == 0);
    }

    task_runner->__invalid = false;
}

void _ntg_task_runner_deinit(ntg_task_runner* task_runner)
{
    assert(task_runner != NULL);

    pthread_mutex_lock(&task_runner->__lock);
    task_runner->__init = false;
    pthread_cond_broadcast(&task_runner->__cond);
    pthread_mutex_unlock(&task_runner->__lock);

    size_t i;
    for(i = 0; i < task_runner->__thread_count; i++)
    {
        pthread_join(task_runner->__threads[i], NULL);
    }

    task_runner->__platform = NULL;

    pthread_mutex_destroy(&task_runner->__lock);
    pthread_cond_destroy(&task_runner->__cond);

    ntg_task_list_deinit(&task_runner->__tasks, NULL);

    task_runner->__running = 0;
    task_runner->__invalid = true;
}

bool _ntg_task_runner_is_running(ntg_task_runner* task_runner)
{
    assert(task_runner != NULL);

    size_t running = 0;

    pthread_mutex_lock(&task_runner->__lock);

    assert(task_runner->__init);
    running = task_runner->__running;

    pthread_mutex_unlock(&task_runner->__lock);

    return (running > 0);
}

void _ntg_task_runner_execute(ntg_task_runner* task_runner, struct ntg_task task)
{
    assert(task_runner != NULL);
    assert(task.task_fn != NULL);

    pthread_mutex_lock(&task_runner->__lock);

    if(task_runner->__invalid)
    {
        pthread_mutex_unlock(&task_runner->__lock);
        return;
    }

    assert(task_runner->__init);
    ntg_task_list_pushb(&task_runner->__tasks, task, NULL);
    pthread_cond_broadcast(&task_runner->__cond);

    pthread_mutex_unlock(&task_runner->__lock);
}

void _ntg_task_runner_invalidate(ntg_task_runner* task_runner)
{
    assert(task_runner != NULL);

    pthread_mutex_lock(&task_runner->__lock);
    task_runner->__invalid = true;
    pthread_mutex_unlock(&task_runner->__lock);
}

static void* worker_fn(void* _data)
{
    ntg_task_runner* runner = (ntg_task_runner*)_data;

    while(true)
    {
        pthread_mutex_lock(&runner->__lock);

        while(runner->__init && (runner->__tasks.size == 0))
        {
            pthread_cond_wait(&runner->__cond, &runner->__lock);
        }

        if(!runner->__init)
        {
            pthread_mutex_unlock(&runner->__lock);
            break;
        }

        struct ntg_task task = *(runner->__tasks.head->data);
        ntg_task_list_popf(&runner->__tasks, NULL);
        (runner->__running)++;

        pthread_mutex_unlock(&runner->__lock);

        task.task_fn(task.data, runner->__platform);

        pthread_mutex_lock(&runner->__lock);

        (runner->__running)--;

        pthread_mutex_unlock(&runner->__lock);
    }

    return NULL;
}
