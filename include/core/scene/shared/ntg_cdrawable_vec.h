#ifndef _NTG_CDRAWABLE_VEC_H_
#define _NTG_CDRAWABLE_VEC_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_drawable_vec ntg_drawable_vec;
typedef struct ntg_cdrawable_vec_view ntg_cdrawable_vec_view;

typedef struct ntg_cdrawable_vec
{
    const ntg_drawable** _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
} ntg_cdrawable_vec;

void __ntg_cdrawable_vec_init__(ntg_cdrawable_vec* vec);
void __ntg_cdrawable_vec_deinit__(ntg_cdrawable_vec* vec);

ntg_cdrawable_vec* ntg_cdrawable_vec_new();
void ntg_cdrawable_vec_destroy(ntg_cdrawable_vec* vec);

void ntg_cdrawable_vec_append(ntg_cdrawable_vec* vec, const ntg_drawable* drawable);
void ntg_cdrawable_vec_remove(ntg_cdrawable_vec* vec, const ntg_drawable* drawable);

/* Returns SIZE_MAX on fail/non-existing. */
size_t ntg_cdrawable_vec_find(const ntg_cdrawable_vec* vec,
        const ntg_drawable* drawable);
bool ntg_cdrawable_vec_contains(const ntg_cdrawable_vec* vec,
        const ntg_drawable* drawable);

struct ntg_cdrawable_vec_view
{
    ntg_cdrawable_vec* __vec;
};

extern const ntg_cdrawable_vec_view NTG_CDRAWABLE_VEC_VIEW_EMPTY;

void __ntg_cdrawable_vec_view_init__(ntg_cdrawable_vec_view* view, ntg_cdrawable_vec* vec);
void __ntg_cdrawable_vec_view_deinit__(ntg_cdrawable_vec_view* view);

size_t ntg_cdrawable_vec_view_count(ntg_cdrawable_vec_view* view);
const ntg_drawable* ntg_cdrawable_vec_view_at(ntg_cdrawable_vec_view* view, size_t pos);

#endif // _NTG_CDRAWABLE_VEC_H_
