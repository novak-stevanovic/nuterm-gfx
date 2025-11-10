#ifndef _NTG_EVENT_SUB_VEC_H_
#define _NTG_EVENT_SUB_VEC_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

struct ntg_event_sub;

typedef struct ntg_event_sub_vec
{
    struct ntg_event_sub* _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
} ntg_event_sub_vec;

void __ntg_event_sub_vec_init__(ntg_event_sub_vec* vec);
void __ntg_event_sub_vec_deinit__(ntg_event_sub_vec* vec);

ntg_event_sub_vec* ntg_event_sub_vec_new();
void ntg_event_sub_vec_destroy(ntg_event_sub_vec* vec);

void ntg_event_sub_vec_append(ntg_event_sub_vec* vec, struct ntg_event_sub sub);
void ntg_event_sub_vec_remove(ntg_event_sub_vec* vec, const void* subscriber);

/* Returns SIZE_MAX on fail/non-existing. */
size_t ntg_event_sub_vec_find(const ntg_event_sub_vec* vec,
        const void* subscriber);
bool ntg_event_sub_vec_contains(const ntg_event_sub_vec* vec,
        const void* subscriber);

#endif // _NTG_EVENT_SUB_VEC_H_
