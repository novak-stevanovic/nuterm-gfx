#ifndef NTG_WIDGET_VEC_H
#define NTG_WIDGET_VEC_H

#include "shared/ntg_shared.h"
#include "thirdparty/genc.h"

GENC_VECTOR_DECLARE(ntg_widget_vec, ntg_widget*, NTG_API)

NTG_API size_t
ntg_widget_vec_find(const struct ntg_widget_vec* vec, ntg_widget* widget);

NTG_API bool
ntg_widget_vec_exists(const struct ntg_widget_vec* vec, ntg_widget* widget);

NTG_API int
ntg_widget_vec_rm(struct ntg_widget_vec* vec, const ntg_widget* widget);

#endif // NTG_WIDGET_VEC_H
