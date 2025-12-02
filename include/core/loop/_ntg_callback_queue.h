#ifndef __NTG_CALLBACK_QUEUE_H__
#define __NTG_CALLBACK_QUEUE_H__

#include <stddef.h>

typedef struct ntg_callback_queue ntg_callback_queue;

/* -------------------------------------------------------------------------- */

ntg_callback_queue* ntg_callback_queue_new();
void ntg_callback_queue_destroy(ntg_callback_queue* callback_queue);

void ntg_callback_queue_run(ntg_callback_queue* callback_queue);

void ntg_callback_queue_append(
        ntg_callback_queue* callback_queue,
        void (*action_fn)(void* data),
        void* data);

#endif // __NTG_CALLBACK_QUEUE_H__
