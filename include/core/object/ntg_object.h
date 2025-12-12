#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include <stdbool.h>
#include <stddef.h>
#include "base/ntg_cell.h"
#include "core/object/shared/ntg_object_vec.h"
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_object ntg_object;

typedef struct ntg_scene ntg_scene;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_dlgt ntg_event_dlgt;
typedef struct ntg_event_sub ntg_event_sub;
typedef struct ntg_padding ntg_padding;
typedef struct ntg_border ntg_border;
typedef struct ntg_object_measures ntg_object_measures;
typedef struct ntg_object_sizes ntg_object_sizes;
typedef struct ntg_object_xys ntg_object_xys;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_loop_ctx ntg_loop_ctx;
typedef struct sarena sarena;
struct nt_key_event;
struct ntg_object_measure_ctx;
struct ntg_object_measure_out;
struct ntg_object_constrain_ctx;
struct ntg_object_constrain_out;
struct ntg_object_arrange_ctx;
struct ntg_object_arrange_out;
struct ntg_object_draw_ctx;
struct ntg_object_draw_out;
struct ntg_object_key_ctx;
struct ntg_object_focus_ctx;
struct ntg_object_unfocus_ctx;

/* -------------------------------------------------------------------------- */
/* PUBLIC DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_object_measure_ctx
{
    const ntg_object_measures* measures;
};

struct ntg_object_measure_out
{
    size_t __placeholder;
};

/* Measures how much space the object would require along one axis,
 * if the size is constrained for the other axis. */
typedef struct ntg_object_measure (*ntg_object_measure_fn)(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena);

/* ------------------------------------------------------ */

struct ntg_object_constrain_ctx
{
    const ntg_object_measures* measures;
};

struct ntg_object_constrain_out
{
    ntg_object_sizes* const sizes;
};

/* Determines the children's sizes. */
typedef void (*ntg_object_constrain_fn)(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        sarena* arena);

/* ------------------------------------------------------ */

struct ntg_object_arrange_ctx
{
    const ntg_object_xys* sizes;
};

struct ntg_object_arrange_out
{
    ntg_object_xys* const pos;
};

/* Determines children positions. */
typedef void (*ntg_object_arrange_fn)(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        sarena* arena);

/* ------------------------------------------------------ */

struct ntg_object_draw_ctx
{
    const ntg_object_xys* sizes;
    const ntg_object_xys* pos;
};

struct ntg_object_draw_out
{
    ntg_object_drawing* const drawing;
};

/* Draws the `object` into `out_object_drawing` */
typedef void (*ntg_object_draw_fn)(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena);

/* ------------------------------------------------------ */

struct ntg_object_key_ctx
{
    ntg_scene* scene;
    ntg_loop_ctx* loop_ctx;
};

/* Returns if the key had been consumed. */
typedef bool (*ntg_object_process_key_fn)(ntg_object* object,
        struct nt_key_event key_event,
        struct ntg_object_key_ctx ctx);

/* ------------------------------------------------------ */

struct ntg_object_focus_ctx
{
    ntg_object* old;
    ntg_scene* scene;
};

typedef void (*ntg_object_focus_fn)(
        ntg_object* object,
        struct ntg_object_focus_ctx ctx);
/* ------------------------------------------------------ */

struct ntg_object_unfocus_ctx
{
    ntg_object* new;
    ntg_scene* scene;
};

typedef void (*ntg_object_unfocus_fn)(
        ntg_object* object,
        struct ntg_object_unfocus_ctx ctx);

typedef void (*ntg_object_deinit_fn)(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

void ntg_object_deinit(ntg_object* object);

/* ------------------------------------------------------ */
/* IDENTITY */
/* ------------------------------------------------------ */

unsigned int ntg_object_get_type(const ntg_object* object);
bool ntg_object_is_widget(const ntg_object* object);
bool ntg_object_is_decorator(const ntg_object* object);
unsigned int ntg_object_get_id(const ntg_object* object);

/* ------------------------------------------------------ */
/* OBJECT TREE */
/* ------------------------------------------------------ */

ntg_object* ntg_object_get_group_root(ntg_object* object);

typedef enum ntg_object_parent_opts
{
    NTG_OBJECT_PARENT_INCL_DECOR,
    NTG_OBJECT_PARENT_EXCL_DECOR
} ntg_object_parent_opts;

/* Gets object's parent node. */
ntg_object* ntg_object_get_parent(ntg_object* object,
        ntg_object_parent_opts opts);

/* Returns base widget for node group(non-decorator). */
ntg_object* ntg_object_get_base_widget(ntg_object* object);

ntg_object_vec_view ntg_object_get_children_(ntg_object* object);
const ntg_object_vec* ntg_object_get_children(const ntg_object* object);

bool ntg_object_is_ancestor(const ntg_object* object, const ntg_object* ancestor);
bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant);

typedef enum ntg_object_perform_mode
{
    NTG_OBJECT_PERFORM_TOP_DOWN,
    NTG_OBJECT_PERFORM_BOTTOM_UP
} ntg_object_perform_mode;

void ntg_object_tree_perform(
        ntg_object* object,
        ntg_object_perform_mode mode,
        void (*perform_fn)(ntg_object* object, void* data),
        void* data);

/* ------------------------------------------------------ */
/* SIZE CONTROL */
/* ------------------------------------------------------ */

struct ntg_xy ntg_object_get_min_size(ntg_object* object);
void ntg_object_set_min_size(ntg_object* object, struct ntg_xy size);

struct ntg_xy ntg_object_get_max_size(ntg_object* object);
void ntg_object_set_max_size(ntg_object* object, struct ntg_xy size);

struct ntg_xy ntg_object_get_grow(const ntg_object* object);
void ntg_object_set_grow(ntg_object* object, struct ntg_xy grow);

/* ------------------------------------------------------ */
/* PADDING & BORDER */
/* ------------------------------------------------------ */

ntg_padding* ntg_object_get_padding(ntg_object* object);
ntg_padding* ntg_object_set_padding(ntg_object* object, ntg_padding* padding);

ntg_border* ntg_object_get_border(ntg_object* object);
ntg_border* ntg_object_set_border(ntg_object* object, ntg_border* border);

ntg_cell ntg_object_get_background(const ntg_object* object);
void ntg_object_set_background(ntg_object* object, ntg_cell background);

/* ------------------------------------------------------ */
/* LAYOUT */
/* ------------------------------------------------------ */

struct ntg_object_measure ntg_object_measure(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t for_size,
        struct ntg_object_measure_ctx ctx,
        struct ntg_object_measure_out* out,
        sarena* arena);

void ntg_object_constrain(
        const ntg_object* object,
        ntg_orientation orientation,
        size_t size,
        struct ntg_object_constrain_ctx ctx,
        struct ntg_object_constrain_out* out,
        sarena* arena);

void ntg_object_arrange(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_arrange_ctx ctx,
        struct ntg_object_arrange_out* out,
        sarena* arena);

void ntg_object_draw(
        const ntg_object* object,
        struct ntg_xy size,
        struct ntg_object_draw_ctx ctx,
        struct ntg_object_draw_out* out,
        sarena* arena);

/* ------------------------------------------------------ */
/* EVENT */
/* ------------------------------------------------------ */

bool ntg_object_feed_key(ntg_object* object,
        struct nt_key_event key_event,
        struct ntg_object_key_ctx ctx);

void ntg_object_on_focus(ntg_object* object, struct ntg_object_focus_ctx ctx);

void ntg_object_on_unfocus(ntg_object* object, struct ntg_object_unfocus_ctx ctx);

ntg_listenable* ntg_object_get_listenable(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* PROTECTED DEFINITIONS */
/* -------------------------------------------------------------------------- */

struct ntg_object
{
    unsigned int __type;
    unsigned int __id;

    struct
    {
        ntg_object* __parent;
        ntg_object_vec* __children;
    };

    struct
    {
        struct ntg_xy __min_size, __max_size, __grow;
    };

    struct
    {
        ntg_cell __background;
    };

    struct
    {
        ntg_object_measure_fn __measure_fn;
        ntg_object_constrain_fn __constrain_fn;
        ntg_object_arrange_fn __arrange_fn;
        ntg_object_draw_fn __draw_fn;
        ntg_object_process_key_fn __process_key_fn;
        ntg_object_focus_fn __on_focus_fn;
        ntg_object_unfocus_fn __on_unfocus_fn;
        ntg_object_deinit_fn __deinit_fn;
    };

    ntg_event_dlgt* _delegate;

    void* _data;
};


/* -------------------------------------------------------------------------- */
/* PROTECTED API */
/* -------------------------------------------------------------------------- */

void __ntg_object_init__(ntg_object* object,
        unsigned int type,
        ntg_object_measure_fn measure_fn,
        ntg_object_constrain_fn constrain_fn,
        ntg_object_arrange_fn arrange_fn,
        ntg_object_draw_fn draw_fn,
        ntg_object_process_key_fn process_key_fn,
        ntg_object_focus_fn on_focus_fn,
        ntg_object_unfocus_fn on_unfocus_fn,
        ntg_object_deinit_fn deinit_fn,
        void* data);
void __ntg_object_deinit__(ntg_object* object);

void _ntg_object_add_child(ntg_object* object, ntg_object* child);
void _ntg_object_rm_child(ntg_object* object, ntg_object* child);

#endif // _NTG_OBJECT_H_
