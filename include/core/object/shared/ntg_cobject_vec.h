#ifndef _NTG_COBJECT_VECTOR_H_
#define _NTG_COBJECT_VECTOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct ntg_object ntg_object;

typedef struct ntg_cobject_vec
{
    const ntg_object** _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
} ntg_cobject_vec;

void _ntg_cobject_vec_init_(ntg_cobject_vec* vec);
void _ntg_cobject_vec_deinit_(ntg_cobject_vec* vec);

ntg_cobject_vec* ntg_cobject_vec_new();
void ntg_cobject_vec_destroy(ntg_cobject_vec* vec);

void ntg_cobject_vec_append(ntg_cobject_vec* vec, const ntg_object* object);
void ntg_cobject_vec_remove(ntg_cobject_vec* vec, const ntg_object* object);
void ntg_cobject_vec_empty(ntg_cobject_vec* vec);

size_t ntg_cobject_vec_find(const ntg_cobject_vec* vec,
        const ntg_object* object);
bool ntg_cobject_vec_contains(const ntg_cobject_vec* vec,
        const ntg_object* object);

typedef struct ntg_cobject_vec_view
{
    ntg_cobject_vec* __vec;
} ntg_cobject_vec_view;

ntg_cobject_vec_view ntg_cobject_vec_view_new(ntg_cobject_vec* vec);

size_t ntg_cobject_vec_view_count(ntg_cobject_vec_view* view);
const ntg_object* ntg_cobject_vec_view_at(ntg_cobject_vec_view* view, size_t pos);

#endif // _NTG_COBJECT_VECTOR_H_
