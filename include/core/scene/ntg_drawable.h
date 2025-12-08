#ifndef _NTG_DRAWABLE_H_
#define _NTG_DRAWABLE_H_

#include "nt_event.h"
#include "shared/ntg_xy.h"

typedef struct ntg_drawable ntg_drawable;
typedef struct ntg_scene ntg_scene;

typedef struct ntg_measure_ctx ntg_measure_ctx;
typedef struct ntg_constrain_ctx ntg_constrain_ctx;
typedef struct ntg_constrain_out ntg_constrain_out;
typedef struct ntg_arrange_ctx ntg_arrange_ctx;
typedef struct ntg_arrange_out ntg_arrange_out;
typedef struct ntg_draw_ctx ntg_draw_ctx;
typedef struct ntg_drawing ntg_drawing;
typedef struct ntg_loop_ctx ntg_loop_ctx;
typedef struct sarena sarena;

typedef struct ntg_drawable_vec ntg_drawable_vec;
typedef struct ntg_drawable_vec_view ntg_drawable_vec_view;

/* Measures how much space the object would require along one axis,
 * if the size is constrained for the other axis. */
typedef struct ntg_measure_out (*ntg_measure_fn)(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t for_size,
        const ntg_measure_ctx* ctx,
        sarena* arena);

/* Determines the children's sizes. */
typedef void (*ntg_constrain_fn)(
        const ntg_drawable* drawable,
        ntg_orientation orientation,
        size_t size,
        const ntg_constrain_ctx* constrain_ctx,
        const ntg_measure_ctx* measure_ctx,
        ntg_constrain_out* out,
        sarena* arena);

/* Determines children positions. */
typedef void (*ntg_arrange_fn)(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        const ntg_arrange_ctx* ctx,
        ntg_arrange_out* out,
        sarena* arena);

/* Draws the `drawable` into `out_drawing` */
typedef void (*ntg_draw_fn)(
        const ntg_drawable* drawable,
        struct ntg_xy size,
        ntg_drawing* out_drawing,
        sarena* arena);

typedef ntg_drawable_vec_view (*ntg_get_children_fn_)(
        ntg_drawable* drawable);
typedef const ntg_drawable_vec* (*ntg_get_children_fn)(
        const ntg_drawable* drawable);

typedef ntg_drawable* (*ntg_get_parent_fn_)(
        ntg_drawable* drawable);
typedef const ntg_drawable* (*ntg_get_parent_fn)(
        const ntg_drawable* drawable);

struct ntg_process_key_ctx
{
    ntg_scene* scene;
    ntg_loop_ctx* loop_ctx;
};

/* Returns if the key had been consumed. */
typedef bool (*ntg_process_key_fn)(
        ntg_drawable* drawable,
        struct nt_key_event key_event,
        struct ntg_process_key_ctx ctx);

struct ntg_focus_ctx
{
    ntg_drawable* old;
    ntg_scene* scene;
};

typedef void (*ntg_on_focus_fn)(
        ntg_drawable* drawable,
        struct ntg_focus_ctx ctx);

struct ntg_unfocus_ctx
{
    ntg_drawable* new;
    ntg_scene* scene;
};

typedef void (*ntg_on_unfocus_fn)(
        ntg_drawable* drawable,
        struct ntg_unfocus_ctx ctx);

struct ntg_drawable
{
    /* Private field */
    void* __user;

    /* Protected, read-only fields */
    ntg_measure_fn _measure_fn_;
    ntg_constrain_fn _constrain_fn_;
    ntg_arrange_fn _arrange_fn_;
    ntg_draw_fn _draw_fn_;
    ntg_process_key_fn _process_key_fn_;
    ntg_on_focus_fn _on_focus_fn_;
    ntg_on_unfocus_fn _on_unfocus_fn_;
    ntg_get_children_fn_ _get_children_fn__;
    ntg_get_children_fn _get_children_fn_;
    ntg_get_parent_fn_ _get_parent_fn__;
    ntg_get_parent_fn _get_parent_fn_;
};

void __ntg_drawable_init__(ntg_drawable* drawable, /* non-NULL */
        void* user, /* non-NULL */
        ntg_measure_fn measure_fn, /* non-NULL */
        ntg_constrain_fn constrain_fn, /* non-NULL */
        ntg_arrange_fn arrange_fn, /* non-NULL */
        ntg_draw_fn draw_fn, /* non-NULL */
        ntg_get_children_fn_ get_children_fn_, /* non-NULL */
        ntg_get_children_fn get_children_fn, /* non-NULL */
        ntg_get_parent_fn_ get_parent_fn_, /* non-NULL */
        ntg_get_parent_fn get_parent_fn, /* non-NULL */
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn,
        ntg_on_unfocus_fn on_unfocus_fn);
void __ntg_drawable_deinit__(ntg_drawable* drawable);

const void* ntg_drawable_user(const ntg_drawable* drawable);
void* ntg_drawable_user_(ntg_drawable* drawable);

bool ntg_drawable_is_ancestor(const ntg_drawable* drawable,
        const ntg_drawable* ancestor);
bool ntg_drawable_is_descendant(const ntg_drawable* drawable,
        const ntg_drawable* descendant);

typedef enum ntg_drawable_perform_mode
{
    NTG_DRAWABLE_PERFORM_TOP_DOWN,
    NTG_DRAWABLE_PERFORM_BOTTOM_UP
} ntg_drawable_perform_mode;

void ntg_drawable_tree_perform(ntg_drawable* drawable,
        ntg_drawable_perform_mode mode,
        void (*perform_fn)(ntg_drawable* drawable, void* data),
        void* data);

#endif // _NTG_DRAWABLE_H_
