#include <assert.h>
#include "core/loop/_ntg_loop.h"
#include "core/entity/ntg_entity_type.h"
#include "core/entity/ntg_entity.h"
#include "core/entity/ntg_entity_type.h"
#include <pthread.h>
#include <string.h>

void _ntg_platform_init(ntg_platform* platform)
{
    assert(platform != NULL);

    pthread_mutex_init(&platform->__lock, NULL);

    ntg_ptask_list_init(&platform->__tasks);
}

void _ntg_platform_deinit(ntg_platform* platform)
{
    assert(platform != NULL);

    pthread_mutex_destroy(&platform->__lock);

    ntg_ptask_list_deinit(&platform->__tasks);
}

void _ntg_platform_execute_later(ntg_platform* platform, struct ntg_ptask task)
{
    assert(platform != NULL);

    pthread_mutex_lock(&platform->__lock);

    ntg_ptask_list_push_back(&platform->__tasks, task);

    pthread_mutex_unlock(&platform->__lock);
}

void _ntg_platform_execute_all(ntg_platform* platform, ntg_loop_ctx* loop_ctx)
{
    assert(platform != NULL);
    assert(loop_ctx != NULL);

    struct ntg_ptask_list_node* it_node = platform->__tasks._head;

    while(it_node != NULL)
    {
        it_node->data->task_fn(it_node->data->data, loop_ctx);
        it_node = it_node->next;

        ntg_ptask_list_pop_front(&platform->__tasks);
    }
}

void _ntg_task_runner_deinit(ntg_task_runner* runner)
{
    assert(runner != NULL);

    pthread_mutex_lock(&runner->__lock);
    runner->__running = false;
    pthread_cond_broadcast(&runner->__cond);
    pthread_mutex_unlock(&runner->__lock);

    size_t i;
    for(i = 0; i < runner->__thread_count; i++)
    {
        pthread_join(runner->__threads[i], NULL);
    }

    runner->__platform = NULL;

    pthread_mutex_destroy(&runner->__lock);
    pthread_cond_destroy(&runner->__cond);

    ntg_task_list_deinit(&runner->__tasks);
}

static void* worker_fn(void* data)
{
    ntg_task_runner* runner = (ntg_task_runner*)data;

    while(true)
    {
        pthread_mutex_lock(&runner->__lock);

        while(runner->__running && (runner->__tasks._count == 0))
        {
            pthread_cond_wait(&runner->__cond, &runner->__lock);
        }

        if(!runner->__running)
        {
            pthread_mutex_unlock(&runner->__lock);
            break;
        }

        struct ntg_task task = *(runner->__tasks._head->data);
        ntg_task_list_pop_front(&runner->__tasks);

        pthread_mutex_unlock(&runner->__lock);

        task.task_fn(task.data, runner->__platform);

    }

    return NULL;
}

void _ntg_task_runner_init(ntg_task_runner* runner,
        ntg_platform* platform, unsigned int worker_threads)
{
    assert(runner != NULL);
    assert(platform != NULL);
    assert(worker_threads < NTG_LOOP_WORKER_THREADS_MAX);

    runner->__platform = platform;
    runner->__thread_count = worker_threads;

    pthread_mutex_init(&runner->__lock, NULL);
    pthread_cond_init(&runner->__cond, NULL);

    runner->__running = true;

    int status;
    size_t i;
    for(i = 0; i < worker_threads; i++)
    {
        status = pthread_create(&(runner->__threads[i]), NULL, worker_fn, runner);
        assert(status == 0);
    }

    ntg_task_list_init(&runner->__tasks);
}

void _ntg_task_runner_execute(ntg_task_runner* task_runner, struct ntg_task task)
{
    assert(task_runner != NULL);
    assert(task.task_fn != NULL);

    pthread_mutex_lock(&task_runner->__lock);

    ntg_task_list_push_back(&task_runner->__tasks, task);
    pthread_cond_broadcast(&task_runner->__cond);

    pthread_mutex_unlock(&task_runner->__lock);
}

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
