#ifndef _NTG_OBJECT_VEC_H_
#define _NTG_OBJECT_VEC_H_

#include "shared/ntg_typedef.h"

/* -------------------------------------------------------------------------- */

struct ntg_object_vec
{
    ntg_object** _data;
    size_t _count;
    size_t __capacity;
    size_t __grow_factor;
};

void ntg_object_vec_init(ntg_object_vec* vec);
void ntg_object_vec_deinit(ntg_object_vec* vec);

void ntg_object_vec_add(ntg_object_vec* vec, ntg_object* object);
void ntg_object_vec_rm(ntg_object_vec* vec, ntg_object* object);
void ntg_object_vec_empty(ntg_object_vec* vec);

size_t ntg_object_vec_find(
        const ntg_object_vec* vec,
        const ntg_object* object);
bool ntg_object_vec_has(
        const ntg_object_vec* vec,
        const ntg_object* object);

struct ntg_object_vecv
{
    ntg_object* const * data;
    size_t count;
};

struct ntg_object_vecv ntg_object_vecv_new(ntg_object_vec* vec);

/* -------------------------------------------------------------------------- */

struct ntg_const_object_vec
{
    const ntg_object** _data;
    size_t _count;
    size_t __capacity;
    size_t __grow_factor;
};

void ntg_const_object_vec_init(ntg_const_object_vec* vec);
void _ntg_const_object_vec_deinit_(ntg_const_object_vec* vec);

void ntg_const_object_vec_add(ntg_const_object_vec* vec, const ntg_object* object);
void ntg_const_object_vec_rm(ntg_const_object_vec* vec, const ntg_object* object);
void ntg_const_object_vec_empty(ntg_const_object_vec* vec);

size_t ntg_const_object_vec_find(
        const ntg_const_object_vec* vec,
        const ntg_object* object);
bool ntg_const_object_vec_has(
        const ntg_const_object_vec* vec,
        const ntg_object* object);

struct ntg_const_object_vecv
{
    const ntg_object* const * data;
    size_t count;
};

struct ntg_const_object_vecv ntg_const_object_vecv_new(ntg_const_object_vec* vec);
struct ntg_const_object_vecv ntg_const_object_vecv_new_(const ntg_object_vec* vec);

#endif // _NTG_OBJECT_VEC_H_
