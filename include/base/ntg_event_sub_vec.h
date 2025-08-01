#ifndef _NTG_EVENT_SUB_VEC_H_
#define _NTG_EVENT_SUB_VEC_H_

#include <stddef.h>
#include <sys/types.h>

struct ntg_event_sub;

typedef struct ntg_event_sub_vec
{
    size_t _count;
    size_t __capacity;
    struct ntg_event_sub* _data;
} ntg_event_sub_vec;

void __ntg_event_sub_vec_init__(ntg_event_sub_vec* vec);
void __ntg_event_sub_vec_deinit__(ntg_event_sub_vec* vec);

ntg_event_sub_vec* ntg_event_sub_vec_new();
void ntg_event_sub_vec_destroy(ntg_event_sub_vec* vec);

void ntg_event_sub_vec_append(ntg_event_sub_vec* vec, struct ntg_event_sub sub);

ssize_t ntg_event_sub_vec_find_sub(const ntg_event_sub_vec* vec,
        const void* subscriber);

void ntg_event_sub_vec_remove_sub(const ntg_event_sub_vec* vec,
        const void* subscriber);

#endif // _NTG_EVENT_SUB_VEC_H_
