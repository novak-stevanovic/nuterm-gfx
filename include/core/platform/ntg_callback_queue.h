#ifndef _NTG_CALLBACK_QUEUE_H_
#define _NTG_CALLBACK_QUEUE_H_

#include <stddef.h>

typedef struct ntg_callback_queue ntg_callback_queue;

typedef struct ntg_callback_queue_channel ntg_callback_queue_channel;

/* -------------------------------------------------------------------------- */

ntg_callback_queue* ntg_callback_queue_new();
void ntg_callback_queue_destroy(ntg_callback_queue* callback_queue);

void ntg_callback_queue_run(ntg_callback_queue* callback_queue);

ntg_callback_queue_channel* ntg_callback_queue_get_channel(
        ntg_callback_queue* callback_queue);

/* -------------------------------------------------------------------------- */

/* Copies requests's data. */
void ntg_callback_queue_channel_append(
        ntg_callback_queue_channel* channel,
        void (*action_fn)(void* data),
        void* data,
        size_t request_data_size);

#endif // _NTG_CALLBACK_QUEUE_H_
