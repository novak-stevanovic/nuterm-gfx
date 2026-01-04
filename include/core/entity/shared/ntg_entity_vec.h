#ifndef _NTG_ENTITY_VEC_H_
#define _NTG_ENTITY_VEC_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#include "shared/ntg_typedef.h"

/* -------------------------------------------------------------------------- */

struct ntg_entity_vec
{
    ntg_entity** _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
};

void _ntg_entity_vec_init_(ntg_entity_vec* vec);
void _ntg_entity_vec_deinit_(ntg_entity_vec* vec);

ntg_entity_vec* ntg_entity_vec_new();
void ntg_entity_vec_destroy(ntg_entity_vec* vec);

void ntg_entity_vec_append(ntg_entity_vec* vec, ntg_entity* entity);
void ntg_entity_vec_remove(ntg_entity_vec* vec, ntg_entity* entity);
void ntg_entity_vec_empty(ntg_entity_vec* vec);

size_t ntg_entity_vec_find(
        const ntg_entity_vec* vec,
        const ntg_entity* entity);
bool ntg_entity_vec_contains(
        const ntg_entity_vec* vec,
        const ntg_entity* entity);

struct ntg_entity_vecv
{
    ntg_entity* const * data;
    size_t count;
};

struct ntg_entity_vecv ntg_entity_vecv_new(ntg_entity_vec* vec);

/* -------------------------------------------------------------------------- */

struct ntg_const_entity_vec
{
    const ntg_entity** _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
};

void _ntg_const_entity_vec_init_(ntg_const_entity_vec* vec);
void _ntg_const_entity_vec_deinit_(ntg_const_entity_vec* vec);

ntg_const_entity_vec* ntg_const_entity_vec_new();
void ntg_const_entity_vec_destroy(ntg_const_entity_vec* vec);

void ntg_const_entity_vec_append(ntg_const_entity_vec* vec, const ntg_entity* entity);
void ntg_const_entity_vec_remove(ntg_const_entity_vec* vec, const ntg_entity* entity);
void ntg_const_entity_vec_empty(ntg_const_entity_vec* vec);

size_t ntg_const_entity_vec_find(
        const ntg_const_entity_vec* vec,
        const ntg_entity* entity);
bool ntg_const_entity_vec_contains(
        const ntg_const_entity_vec* vec,
        const ntg_entity* entity);

struct ntg_const_entity_vecv
{
    const ntg_entity* const * data;
    size_t count;
};

struct ntg_const_entity_vecv ntg_const_entity_vecv_new(ntg_const_entity_vec* vec);
struct ntg_const_entity_vecv ntg_const_entity_vecv_new_(ntg_entity_vec* vec);

#endif // _NTG_ENTITY_VEC_H_

