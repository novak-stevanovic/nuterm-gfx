#ifndef _NTG_OBJECT_FX_VEC_H_
#define _NTG_OBJECT_FX_VEC_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct ntg_object_fx_vec
{
    struct ntg_object_fx* _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
} ntg_object_fx_vec;

void __ntg_object_fx_vec_init__(ntg_object_fx_vec* vec);
void __ntg_object_fx_vec_deinit__(ntg_object_fx_vec* vec);

ntg_object_fx_vec* ntg_object_fx_vec_new();
void ntg_object_fx_vec_destroy(ntg_object_fx_vec* vec);

void ntg_object_fx_vec_append(ntg_object_fx_vec* vec, struct ntg_object_fx object_fx);
void ntg_object_fx_vec_remove(ntg_object_fx_vec* vec, struct ntg_object_fx object_fx);

size_t ntg_object_fx_vec_find(const ntg_object_fx_vec* vec,
        struct ntg_object_fx object_fx);
bool ntg_object_fx_vec_contains(const ntg_object_fx_vec* vec,
        struct ntg_object_fx object_fx);

typedef struct ntg_object_fx_vec_view
{
    ntg_object_fx_vec* __vec;
} ntg_object_fx_vec_view;

ntg_object_fx_vec_view ntg_object_fx_vec_view_new(ntg_object_fx_vec* vec);

size_t ntg_object_fx_vec_view_count(ntg_object_fx_vec_view* view);
struct ntg_object_fx ntg_object_fx_vec_view_at(ntg_object_fx_vec_view* view, size_t pos);

#endif // _NTG_OBJECT_FX_VEC_H_
