#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include "base/ntg_event.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"
#include "base/ntg_event.h"

/* -------------------------------------------------------------------------- */
/* DECLARATIONS & DEFINITIONS */
/* -------------------------------------------------------------------------- */

typedef struct ntg_object ntg_object;
typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_constr_object ntg_constraints_object;
typedef struct ntg_positions_object ntg_positions_object;
typedef struct ntg_object_drawing ntg_object_drawing;

/* ---------------------------------------------------------------- */

typedef struct ntg_xy (*ntg_natural_size_fn)(ntg_object* object);

typedef ntg_constraints_object* (*ntg_constrain_fn)(ntg_object* object);

typedef struct ntg_xy (*ntg_measure_fn)(ntg_object* object);

typedef ntg_positions_object* (*ntg_arrange_children_fn)(ntg_object* object);

typedef void (*ntg_arrange_drawing_fn)(ntg_object* object, ntg_object_drawing* drawing);

typedef bool (*ntg_object_process_key_fn)(ntg_object* object,
        struct nt_key_event key_event);

typedef enum ntg_object_type
{
    NTG_OBJECT_WIDGET,
    NTG_OBJECT_DECORATOR
} ntg_object_type;

typedef enum ntg_object_get_parent_mode
{
    NTG_OBJECT_GET_PARENT_DEFAULT,
    NTG_OBJECT_GET_PARENT_DECORATOR
} ntg_object_get_parent_mode;

/* -------------------------------------------------------------------------- */
/* PUBLIC API */
/* -------------------------------------------------------------------------- */

/* Gets object's parent node. */
ntg_object* ntg_object_get_parent(ntg_object* object, ntg_object_get_parent_mode mode);

/* Gets decorator for object. If object is top decorator, returns NULL. */
ntg_object* ntg_object_get_decorator(ntg_object* object);
/* Gets top decorator for an object. Works for decorators as well. */
ntg_object* ntg_object_get_top_decorator(ntg_object* object);

/* Returns base widget for node group(non-decorator). */
ntg_object* ntg_object_get_base_widget(ntg_object* object);

/* Returns children of object. These can include object decorators. */
ntg_object_vec* ntg_object_get_children(ntg_object* object);

/* ---------------------------------------------------------------- */

struct ntg_xy ntg_object_get_natural_size(const ntg_object* object);
struct ntg_constr ntg_object_get_constraints(const ntg_object* object);
struct ntg_constr ntg_object_get_adjusted_constraints(const ntg_object* object);
struct ntg_xy ntg_object_get_size(const ntg_object* object);
struct ntg_xy ntg_object_get_position(const ntg_object* object);
const ntg_object_drawing* ntg_object_get_drawing(const ntg_object* object);

/* ---------------------------------------------------------------- */

void ntg_object_listen(ntg_object* object, struct ntg_event_sub subscription);
void ntg_object_stop_listening(ntg_object* object, ntg_object* listener);

/* -------------------------------------------------------------------------- */
/* DEFINITION */
/* -------------------------------------------------------------------------- */

struct ntg_object
{
    struct
    {
        ntg_object* __parent;
        ntg_object_vec* __children;
    };

    struct
    {
        ntg_natural_size_fn __natural_size_fn;
        ntg_constrain_fn __constrain_fn;
        ntg_measure_fn __measure_fn;
        ntg_arrange_children_fn __arrange_children_fn;
        ntg_arrange_drawing_fn __arrange_drawing_fn;
    };

    struct
    {
        struct ntg_xy __natural_size;

        struct ntg_constr __constraints;
        struct ntg_constr __adjusted_constraints;

        struct ntg_xy __size;

        struct ntg_xy __position;

        ntg_object_drawing* __drawing;
    };

    struct
    {
        struct ntg_xy __min_size, __max_size;
    };

    ntg_object_process_key_fn __process_key_fn;

    // ntg_scene* _scene;

    ntg_listenable __listenable;
};

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED API */
/* -------------------------------------------------------------------------- */

void __ntg_object_init__(ntg_object* object,
        ntg_natural_size_fn natural_size_fn,
        ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn,
        ntg_arrange_children_fn arrange_children_fn,
        ntg_arrange_drawing_fn arrange_drawing_fn);

void __ntg_object_deinit__(ntg_object* object);

void _ntg_object_set_process_key_fn(ntg_object* object,
        ntg_object_process_key_fn process_key_fn);

ntg_object_drawing* _ntg_object_get_drawing(ntg_object* object);

#endif // _NTG_OBJECT_H_
