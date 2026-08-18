#include "shared/ntg_shared_internal.h"
#include <pthread.h>
#include <stdlib.h>

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

struct ntg_task_cancel_token
{
    ntg_task_runner* runner;
    bool cancelled;
};

struct ntg_task
{
    void (*task_fn)(void* data, ntg_task_cancel_token* cancel);
    void* data;
    unsigned int priority;
};

GENC_LIST_INLINE(ntg_task_list, struct ntg_task)

struct ntg_task_runner
{
    pthread_t* threads;
    size_t thread_count;

    struct ntg_task_list task_list;

    unsigned int running;
    bool stopping;

    /* Universal token for all tasks */
    ntg_task_cancel_token cancel_token;

    /* Locks the containing struct */
    pthread_mutex_t lock;

    pthread_cond_t cond;
};

static void* worker_fn(void* _runner);

/* ========================================================================== */
/* PUBLIC */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* FUNCTIONS */
/* -------------------------------------------------------------------------- */

/* ------------------------------------------------------ */
/* TASK RUNNER */
/* ------------------------------------------------------ */

int ntg_task_runner_new(unsigned int workers, ntg_task_runner** out_runner)
{
    if((workers == 0) || !out_runner)
        return NTG_ERR_INV_ARG;

    *out_runner = NULL;

    ntg_task_runner* new = malloc(sizeof(struct ntg_task_runner));
    if(!new)
        return NTG_ERR_ALLOC_FAIL;

    int _status;

    /* Zero initialize */

    (*new) = (struct ntg_task_runner) {0};

    /* Task list */


    /* Mutex & cond */

    pthread_mutex_init(&new->lock, NULL);
    pthread_cond_init(&new->cond, NULL);

    /* Threads */

    new->threads = malloc(sizeof(pthread_t) * workers);
    if(!new->threads)
    {
        (void)ntg_task_runner_destroy(new);
        return NTG_ERR_ALLOC_FAIL;
    } 

    /* Spawn worker threads */

    size_t i;
    for(i = 0; i < workers; i++)
    {
        _status = pthread_create(&(new->threads[i]), NULL, worker_fn, new);
        if(_status) break;
        new->thread_count++;
    }

    /* Thread spawning failed */

    if(_status)
    {
        (void)ntg_task_runner_destroy(new);
        return NTG_ERR_THREAD_SPAWN;
    }

    /* Init cancel token */

    new->cancel_token = (struct ntg_task_cancel_token) {
        .cancelled = false,
        .runner = new
    };

    *out_runner = new;
    return 0;
}

int ntg_task_runner_destroy(ntg_task_runner* runner)
{
    if(!runner)
        return NTG_ERR_INV_ARG;

    pthread_mutex_lock(&runner->lock);
    
    runner->stopping = true;
    runner->cancel_token.cancelled = true;
    pthread_cond_broadcast(&runner->cond);

    pthread_mutex_unlock(&runner->lock);

    size_t i;
    for(i = 0; i < runner->thread_count; i++)
    {
        pthread_join(runner->threads[i], NULL);
    }

    if(runner->threads) free(runner->threads);

    (void)ntg_task_list_deinit(&runner->task_list);
    pthread_mutex_destroy(&runner->lock);
    pthread_cond_destroy(&runner->cond);

    free(runner);
    return 0;
}

int ntg_task_runner_execute(
        ntg_task_runner* runner,
        void (*task_fn)(void* data, ntg_task_cancel_token* cancel),
        void* data,
        unsigned int priority)
{
    if(!runner || !task_fn)
        return NTG_ERR_INV_ARG;

    int _status;

    struct ntg_task task = {
        .task_fn = task_fn,
        .data = data,
        .priority = priority
    };

    /* Lock */

    pthread_mutex_lock(&runner->lock);

    struct ntg_task_list_node* it = runner->task_list.head;
    struct ntg_task* it_data;
    while(it != NULL)
    {
        it_data = &it->data;

        if(priority > it_data->priority)
        {
            _status = ntg_task_list_ins_before(&runner->task_list, task, it);
            if(_status)
            {
                pthread_mutex_unlock(&runner->lock);
                return NTG_ERR_ALLOC_FAIL;
            }

            break;
        }

        it = it->next;
    }

    if(it == NULL) /* If at the end, no insertion happened */
    {
        _status = ntg_task_list_pushb(&runner->task_list, task);
        if(_status)
        {
            pthread_mutex_unlock(&runner->lock);
            return NTG_ERR_ALLOC_FAIL;
        }
    }

    (runner->running)++;

    /* Wake up workers */

    pthread_cond_signal(&runner->cond);

    /* Unlock */

    pthread_mutex_unlock(&runner->lock);
    return 0;
}

bool ntg_task_runner_is_active(ntg_task_runner* runner)
{
    if(!runner) return false;
    
    bool active;

    pthread_mutex_lock(&runner->lock);
    active = (runner->running > 0); 
    pthread_mutex_unlock(&runner->lock);

    return active;
}

/* ------------------------------------------------------ */
/* TASK CANCEL TOKEN */
/* ------------------------------------------------------ */

bool ntg_task_cancel_token_stopped(ntg_task_cancel_token* cancel)
{
    if(!cancel) return false;

    ntg_task_runner* runner = cancel->runner;
    bool cancelled;
    
    pthread_mutex_lock(&runner->lock);
    cancelled = runner->cancel_token.cancelled;
    pthread_mutex_unlock(&runner->lock);

    return cancelled;
}

/* ========================================================================== */
/* STATIC */
/* ========================================================================== */

static void* worker_fn(void* _runner)
{
    ntg_task_runner* runner = _runner;

    while(true)
    {
        pthread_mutex_lock(&runner->lock);

        while((runner->task_list.size < 1) && !runner->stopping)
        {
            pthread_cond_wait(&runner->cond, &runner->lock);
        }

        if(runner->stopping)
        {
            pthread_mutex_unlock(&runner->lock);
            break;
        }

        /* Retrieve & pop task */

        struct ntg_task task = runner->task_list.head->data;
        (void)ntg_task_list_popf(&runner->task_list);

        pthread_mutex_unlock(&runner->lock);

        /* Execute retrieved task */

        if(task.task_fn)
            task.task_fn(task.data, &runner->cancel_token);

        /* Decrement running task counter */

        pthread_mutex_lock(&runner->lock);
        (runner->running)--;
        pthread_mutex_unlock(&runner->lock);
    }

    return NULL;
}
