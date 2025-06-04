#ifndef _NTG_OBJECT_VEC_H_
#define _NTG_OBJECT_VEC_H_

#include <stddef.h>

#include "object/fwd/ntg_object_fwd.h"

typedef struct ntg_object_vec ntg_object_vec_t;

ntg_object_vec_t* ntg_object_vec_new();
void ntg_object_vec_destroy(ntg_object_vec_t* vec);

void ntg_object_vec_append(ntg_object_vec_t* vec, ntg_object_t* obj);
void ntg_object_vec_remove(ntg_object_vec_t* vec, ntg_object_t* obj);

size_t ntg_object_vec_size(const ntg_object_vec_t* vec);
ntg_object_t* ntg_object_vec_at(const ntg_object_vec_t* vec, size_t pos);

#endif // _NTG_OBJECT_VEC_H_
