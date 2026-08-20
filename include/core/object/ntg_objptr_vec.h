#ifndef NTG_OBJPTR_VEC_H
#define NTG_OBJPTR_VEC_H

#include "shared/ntg_shared.h"
#include "thirdparty/genc.h"

GENC_VECTOR_DECLARE(ntg_objptr_vec, ntg_object*, 1.5, NTG_API)

NTG_API size_t
ntg_objptr_vec_find(const struct ntg_objptr_vec* vec, ntg_object* object);

NTG_API bool
ntg_objptr_vec_exists(const struct ntg_objptr_vec* vec, ntg_object* object);

NTG_API int
ntg_objptr_vec_rm(struct ntg_objptr_vec* vec, const ntg_object* object);

#endif // NTG_OBJPTR_VEC_H
