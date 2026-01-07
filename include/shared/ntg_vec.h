#ifndef _NTG_VEC_H_
#define _NTG_VEC_H_

#include "shared/ntg_typedef.h"

struct ntg_vec
{
    void* _data;
    size_t _count;
    size_t __capacity;
    size_t __grow_factor;
};

void ntg_vec_init(ntg_vec* v, size_t data_size, size_t init_cap, size_t grow_factor);
void ntg_vec_deinit(ntg_vec* vec);

void ntg_vec_add(ntg_vec* vec, const void* data, size_t data_size);
void ntg_vec_ins(ntg_vec* vec, size_t index, const void* data, size_t data_size);

void ntg_vec_rm_at(ntg_vec* vec, size_t index, size_t data_size);
void ntg_vec_rm(ntg_vec* vec, const void* key, ntg_vec_cmp_fn cmp_fn, size_t data_size);
void ntg_vec_empty(ntg_vec* vec);
void ntg_vec_shrink(ntg_vec* vec, size_t data_size);

/* Returns index in [0, count-1] if found, or SIZE_MAX if not found.
 * Uses default memcpy to compare values if `cmp_fn` is NULL. */
size_t ntg_vec_find(ntg_vec* vec, const void* key,
        ntg_vec_cmp_fn cmp_fn, size_t data_size);
bool ntg_vec_has(ntg_vec* vec, const void* key,
        ntg_vec_cmp_fn cmp_fn, size_t data_size);

#endif // _NTG_VEC_H_
