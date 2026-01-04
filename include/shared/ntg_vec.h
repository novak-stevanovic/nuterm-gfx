#ifndef _NTG_VEC_H_
#define _NTG_VEC_H_

#include "shared/ntg_typedef.h"

struct ntg_vec
{
    void* __data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
};

void ntg_vec_init(ntg_vec* v, size_t data_size, size_t init_cap);
void ntg_vec_deinit(ntg_vec* vec);

void ntg_vec_add(ntg_vec* vec, const void* data);
void ntg_vec_ins(ntg_vec* vec, size_t index, const void* data);

void ntg_vec_rm_at(ntg_vec* vec, size_t index);
void ntg_vec_rm(ntg_vec* vec, const void* key, ntg_vec_cmp_fn cmp_fn);
void ntg_vec_empty(ntg_vec* vec);

/* Returns index in [0, count-1] if found, or SIZE_MAX if not found.
 * Uses default memcpy to compare values if `cmp_fn` is NULL. */
size_t ntg_vec_find(ntg_vec* vec, const void* key, ntg_vec_cmp_fn cmp_fn);
bool ntg_vec_has(ntg_vec* vec, const void* key, ntg_vec_cmp_fn cmp_fn);

#endif // _NTG_VEC_H_
