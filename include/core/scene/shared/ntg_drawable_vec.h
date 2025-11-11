#ifndef _NTG_DRAWABLE_VEC_H_
#define _NTG_DRAWABLE_VEC_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_drawable_vec_view ntg_drawable_vec_view;

typedef struct ntg_drawable_vec
{
    ntg_drawable** _data;
    size_t _count;
    size_t __capacity;
    size_t __data_size;
} ntg_drawable_vec;

void __ntg_drawable_vec_init__(ntg_drawable_vec* vec);
void __ntg_drawable_vec_deinit__(ntg_drawable_vec* vec);

ntg_drawable_vec* ntg_drawable_vec_new();
void ntg_drawable_vec_destroy(ntg_drawable_vec* vec);

void ntg_drawable_vec_append(ntg_drawable_vec* vec, ntg_drawable* drawable);
void ntg_drawable_vec_remove(ntg_drawable_vec* vec, ntg_drawable* drawable);

/* Returns SIZE_MAX on fail/non-existing. */
size_t ntg_drawable_vec_find(const ntg_drawable_vec* vec,
        const ntg_drawable* drawable);
bool ntg_drawable_vec_contains(const ntg_drawable_vec* vec,
        const ntg_drawable* drawable);

struct ntg_drawable_vec_view
{
    ntg_drawable_vec* __vec;
};

extern const ntg_drawable_vec_view NTG_DRAWABLE_VEC_VIEW_EMPTY;

void __ntg_drawable_vec_view_init__(ntg_drawable_vec_view* view, ntg_drawable_vec* vec);
void __ntg_drawable_vec_view_deinit__(ntg_drawable_vec_view* view);

size_t ntg_drawable_vec_view_count(ntg_drawable_vec_view* view);
ntg_drawable* ntg_drawable_vec_view_at(ntg_drawable_vec_view* view, size_t pos);

#endif // _NTG_DRAWABLE_VEC_H_
