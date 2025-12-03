#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include "base/ntg_cell.h"
#include "core/object/shared/ntg_object_fx_vec.h"
#include "core/object/shared/ntg_object_vec.h"
#include "core/scene/ntg_drawable.h"
#include "shared/ntg_xy.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

#define NTG_OBJECT(obj_ptr) ((ntg_object*)(obj_ptr))

typedef struct ntg_object ntg_object;
typedef struct ntg_listenable ntg_listenable;
typedef struct ntg_event_delegate ntg_event_delegate;
typedef struct ntg_event_sub ntg_event_sub;
typedef struct ntg_object_fx ntg_object_fx;
typedef struct ntg_object_fx_vec ntg_object_fx_vec;

/* ---------------------------------------------------------------- */

typedef enum ntg_object_get_parent_opts
{
    NTG_OBJECT_GET_PARENT_INCL_DECORATOR,
    NTG_OBJECT_GET_PARENT_EXCL_DECORATOR
} ntg_object_get_parent_opts;

// typedef enum ntg_object_perform_mode
// {
//     NTG_OBJECT_PERFORM_TOP_DOWN,
//     NTG_OBJECT_PERFORM_BOTTOM_UP
// } ntg_object_perform_mode;

typedef void* (*ntg_object_get_fx_interface_fn)(ntg_object* object);

typedef void (*ntg_object_reset_fx_fn)(
        const ntg_object* object,
        void* object_fx_interface);

typedef void (*ntg_object_fx_apply_fn)(
        const ntg_object* object,
        struct ntg_object_fx fx,
        void* object_fx_interface);

struct ntg_object_fx
{
    unsigned int object_type;
    ntg_object_fx_apply_fn apply_fn;

    /* ptr to dynamically allocated data */
    void* data;
};

/* ---------------------------------------------------------------- */

unsigned int ntg_object_get_type(const ntg_object* object);
bool ntg_object_is_widget(const ntg_object* object);
bool ntg_object_is_decorator(const ntg_object* object);

unsigned int ntg_object_get_id(const ntg_object* object);

ntg_object* ntg_object_get_group_root(ntg_object* object);

/* Gets object's parent node. */
ntg_object* ntg_object_get_parent(ntg_object* object,
        ntg_object_get_parent_opts mode);

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

bool ntg_object_has_fx_capabilities(const ntg_object* object);
void ntg_object_apply_fx(ntg_object* object, struct ntg_object_fx fx);
void ntg_object_remove_fx(ntg_object* object, struct ntg_object_fx fx);
const ntg_object_fx_vec* ntg_object_get_fx(const ntg_object* object);

/* ---------------------------------------------------------------- */

ntg_drawable* ntg_object_get_drawable_(ntg_object* object);
const ntg_drawable* ntg_object_get_drawable(const ntg_object* object);

ntg_listenable* ntg_object_get_listenable(ntg_object* object);

/* ---------------------------------------------------------------- */

// void ntg_object_tree_perform(ntg_object* object,
//         ntg_object_perform_mode mode,
//         void (*perform_fn)(ntg_object* object, void* data),
//         void* data);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
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
        ntg_cell __background;
    };

    struct
    {
        struct ntg_xy __min_size, __max_size, __grow;
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

    struct
    {
        ntg_object_get_fx_interface_fn __get_fx_interface_fn;
        ntg_object_reset_fx_fn __reset_fx_fn;
        ntg_object_fx_vec* __fx;
    };

    struct ntg_drawable __drawable;
    ntg_event_delegate* __delegate;

    void* _data;
};

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED API */
/* -------------------------------------------------------------------------- */

void __ntg_object_init__(ntg_object* object,
        unsigned int type,
        ntg_measure_fn measure_fn,
        ntg_constrain_fn constrain_fn,
        ntg_arrange_fn arrange_fn,
        ntg_draw_fn draw_fn,
        ntg_process_key_fn process_key_fn,
        ntg_on_focus_fn on_focus_fn,
        ntg_on_unfocus_fn on_unfocus_fn,
        ntg_object_reset_fx_fn reset_fx_fn,
        ntg_object_get_fx_interface_fn get_fx_interface_fn,
        void* data);

void __ntg_object_deinit__(ntg_object* object);

/* ---------------------------------------------------------------- */

void _ntg_object_set_background(ntg_object* object, ntg_cell background);

/* ---------------------------------------------------------------- */

void _ntg_object_add_child(ntg_object* object, ntg_object* child);
void _ntg_object_rm_child(ntg_object* object, ntg_object* child);

/* ---------------------------------------------------------------- */

#endif // _NTG_OBJECT_H_
