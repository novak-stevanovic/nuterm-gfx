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

void ntg_entity_vec_init(ntg_entity_vec* vec);
void ntg_entity_vec_deinit(ntg_entity_vec* vec);

void ntg_entity_vec_add(ntg_entity_vec* vec, ntg_entity* entity);
void ntg_entity_vec_rm(ntg_entity_vec* vec, ntg_entity* entity);
void ntg_entity_vec_empty(ntg_entity_vec* vec);

size_t ntg_entity_vec_find(const ntg_entity_vec* vec, const ntg_entity* entity);
bool ntg_entity_vec_has(const ntg_entity_vec* vec, const ntg_entity* entity);

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

void ntg_const_entity_vec_init(ntg_const_entity_vec* vec);
void ntg_const_entity_vec_deinit(ntg_const_entity_vec* vec);

void ntg_const_entity_vec_add(ntg_const_entity_vec* vec, const ntg_entity* entity);
void ntg_const_entity_vec_rm(ntg_const_entity_vec* vec, const ntg_entity* entity);
void ntg_const_entity_vec_empty(ntg_const_entity_vec* vec);

size_t ntg_const_entity_vec_find(const ntg_const_entity_vec* vec, const ntg_entity* entity);
bool ntg_const_entity_vec_has(const ntg_const_entity_vec* vec, const ntg_entity* entity);

struct ntg_const_entity_vecv
{
    const ntg_entity* const * data;
    size_t count;
};

struct ntg_const_entity_vecv ntg_const_entity_vecv_new(ntg_const_entity_vec* vec);
struct ntg_const_entity_vecv ntg_const_entity_vecv_new_(ntg_entity_vec* vec);

#endif // _NTG_ENTITY_VEC_H_

