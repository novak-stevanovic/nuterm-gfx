#ifndef _NTG_AUX_OBJECT_VEC_H_
#define _NTG_AUX_OBJECT_VEC_H_

#include <sys/types.h>

typedef struct ntg_object ntg_object;

#define NTG_AUX_OBJECT_Z_INDEX_DEFAULT 1000

struct ntg_aux_object
{
    ntg_object* object;

    uint z_index;
};

typedef struct ntg_aux_object_vec
{
    size_t _count;
    size_t __capacity;
    struct ntg_aux_object* _data;
} ntg_aux_object_vec;

void __ntg_aux_object_vec_init__(ntg_aux_object_vec* vec);
void __ntg_aux_object_vec_deinit__(ntg_aux_object_vec* vec);

ntg_aux_object_vec* ntg_aux_object_vec_new();
void ntg_aux_object_vec_destroy(ntg_aux_object_vec* vec);

void ntg_aux_object_vec_insert(ntg_aux_object_vec* vec,
        struct ntg_aux_object aux_object);

void ntg_aux_object_vec_remove(ntg_aux_object_vec* vec, ntg_object* object);

void ntg_aux_object_vec_empty(ntg_aux_object_vec* vec);

#endif // _NTG_AUX_OBJECT_VEC_H_
