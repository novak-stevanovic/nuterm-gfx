#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include "base/ntg_cell.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/scene/ntg_drawable.h"
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* DECLARATIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_object ntg_object;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef struct ntg_event_sub ntg_event_sub;
typedef struct ntg_object_fx_vec ntg_object_fx_vec;
typedef struct ntg_object_padding ntg_object_padding;
typedef struct ntg_object_border ntg_object_border;

typedef void (*ntg_object_deinit_fn)(ntg_object* object);

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

unsigned int ntg_object_get_type(const ntg_object* object);
bool ntg_object_is_widget(const ntg_object* object);
bool ntg_object_is_decorator(const ntg_object* object);

unsigned int ntg_object_get_id(const ntg_object* object);

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

bool ntg_object_is_ancestor(ntg_object* object, ntg_object* ancestor);
bool ntg_object_is_descendant(ntg_object* object, ntg_object* descendant);

/* ---------------------------------------------------------------- */

void ntg_object_set_min_size(ntg_object* object, struct ntg_xy size);
void ntg_object_set_max_size(ntg_object* object, struct ntg_xy size);

struct ntg_xy ntg_object_get_min_size(ntg_object* object);
struct ntg_xy ntg_object_get_max_size(ntg_object* object);

void ntg_object_set_grow(ntg_object* object, struct ntg_xy grow);
struct ntg_xy ntg_object_get_grow(const ntg_object* object);

/* ---------------------------------------------------------------- */

ntg_object_padding* ntg_object_set_padding(ntg_object* object, ntg_object_padding* padding);
ntg_object_padding* ntg_object_get_padding(ntg_object* object);

ntg_object_border* ntg_object_set_border(ntg_object* object, ntg_object_border* border);
ntg_object_border* ntg_object_get_border(ntg_object* object);

void ntg_object_set_background(ntg_object* object, ntg_cell background);
ntg_cell ntg_object_get_background(const ntg_object* object);

/* ---------------------------------------------------------------- */

ntg_drawable* ntg_object_to_drawable_(ntg_object* object);
const ntg_drawable* ntg_object_to_drawable(const ntg_object* object);

ntg_listenable* ntg_object_get_listenable(ntg_object* object);

/* ---------------------------------------------------------------- */

typedef enum ntg_object_perform_mode
{
    NTG_OBJECT_PERFORM_TOP_DOWN,
    NTG_OBJECT_PERFORM_BOTTOM_UP
} ntg_object_perform_mode;

void ntg_object_tree_perform(ntg_object* object,
        ntg_object_perform_mode mode,
        void (*perform_fn)(ntg_object* object, void* data),
        void* data);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

struct ntg_object
{
    unsigned int __type;
    unsigned int __id;

    struct
    {
        ntg_object* __parent;
        ntg_object_vec* __children;
        ntg_drawable_vec* __children_drawables;
    };

    struct
    {
        struct ntg_xy __min_size, __max_size, __grow;
    };

    struct
    {
        ntg_cell _background_;
    };

    struct
    {
        ntg_measure_fn __wrapped_measure_fn;
        ntg_constrain_fn __wrapped_constrain_fn;
        ntg_arrange_fn __wrapped_arrange_fn;
        ntg_draw_fn __wrapped_draw_fn;
        ntg_process_key_fn __wrapped_process_key_fn;
        ntg_on_focus_fn __wrapped_on_focus_fn;
        ntg_on_unfocus_fn __wrapped_on_unfocus_fn;
    };

    struct ntg_drawable __drawable;
    ntg_event_delegate* _delegate;

    void* _data;
};

/* ---------------------------------------------------------------- */

void __ntg_object_init__(ntg_object* object,
        unsigned int type,
        ntg_measure_fn measure_fn,
        ntg_constrain_fn constrain_fn,
        ntg_arrange_fn arrange_fn,
        ntg_draw_fn draw_fn,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn,
        ntg_on_unfocus_fn on_unfocus_fn,
        void* data);

void __ntg_object_deinit__(ntg_object* object);

/* ---------------------------------------------------------------- */

void _ntg_object_add_child(ntg_object* object, ntg_object* child);
void _ntg_object_rm_child(ntg_object* object, ntg_object* child);
void _ntg_object_rm_children(ntg_object* object);

/* ---------------------------------------------------------------- */

/* Removes the node from tree. Connects the parent with children */
void _ntg_object_rm_node_from_tree(ntg_object* node);

/* ---------------------------------------------------------------- */

#endif // _NTG_OBJECT_H_
