#ifndef _NTG_OBJECT_VECTOR_H_
#define _NTG_OBJECT_VECTOR_H_

#include <stddef.h>

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

#endif // _NTG_OBJECT_VECTOR_H_
