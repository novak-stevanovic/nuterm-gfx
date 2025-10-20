#ifndef _NTG_OBJECT_VECTOR_H_
#define _NTG_OBJECT_VECTOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct ntg_object ntg_object;

typedef struct ntg_object_vec
{
    size_t _count;
    size_t __capacity;
    ntg_object** _data;
} ntg_object_vec;

void __ntg_object_vec_init__(ntg_object_vec* vec);
void __ntg_object_vec_deinit__(ntg_object_vec* vec);

ntg_object_vec* ntg_object_vec_new();
void ntg_object_vec_destroy(ntg_object_vec* vec);

void ntg_object_vec_append(ntg_object_vec* vec, ntg_object* object);
void ntg_object_vec_remove(ntg_object_vec* vec, ntg_object* object);

/* Returns -1 on fail/non-existing. */
ssize_t ntg_object_vec_find(const ntg_object_vec* vec, const ntg_object* object);
bool ntg_object_vec_contains(const ntg_object_vec* vec, const ntg_object* object);

typedef struct ntg_object_vec_view
{
    ntg_object_vec* __vec;
} ntg_object_vec_view;

void __ntg_object_vec_view_init__(ntg_object_vec_view* view, ntg_object_vec* vec);
void __ntg_object_vec_view_deinit__(ntg_object_vec_view* view);

size_t ntg_object_vec_view_count(ntg_object_vec_view* view);
ntg_object* ntg_object_vec_view_at(ntg_object_vec_view* view, size_t pos);

#endif // _NTG_OBJECT_VECTOR_H_
