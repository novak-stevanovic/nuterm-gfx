#include "shared/ntg_lock.h"
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

struct ntg_lock
{
    pthread_mutex_t __mutex;
};

ntg_lock* ntg_lock_new()
{
    ntg_lock* new = (ntg_lock*)malloc(sizeof(ntg_lock));

    pthread_mutex_init(&new->__mutex, NULL);

    return new;
}

void ntg_lock_destroy(ntg_lock* lock)
{
    assert(lock != NULL);

    pthread_mutex_destroy(&lock->__mutex);

    free(lock);
}

void ntg_lock_lock(ntg_lock* lock)
{
    assert(lock != NULL);

    pthread_mutex_lock(&lock->__mutex);
}

void ntg_lock_unlock(ntg_lock* lock)
{
    assert(lock != NULL);

    pthread_mutex_unlock(&lock->__mutex);
}
