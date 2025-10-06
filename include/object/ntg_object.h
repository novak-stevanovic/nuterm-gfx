#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include "base/ntg_cell.h"
#include "base/ntg_event.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"
#include "base/ntg_event.h"
#include "object/shared/ntg_fn.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

#define NTG_OBJECT(obj_ptr) ((ntg_object*)(obj_ptr))

typedef struct ntg_object ntg_object;
typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_scene ntg_scene;

/* ---------------------------------------------------------------- */

typedef enum ntg_object_type
{
    NTG_OBJECT_WIDGET,
    NTG_OBJECT_DECORATOR
} ntg_object_type;

typedef enum ntg_object_get_parent_mode
{
    NTG_OBJECT_GET_PARENT_INCL_DECORATOR,
    NTG_OBJECT_GET_PARENT_EXCL_DECORATOR
} ntg_object_get_parent_mode;

typedef enum ntg_object_perform_mode
{
    NTG_OBJECT_PERFORM_TOP_DOWN,
    NTG_OBJECT_PERFORM_BOTTOM_UP
} ntg_object_perform_mode;

/* ---------------------------------------------------------------- */

ntg_object_type ntg_object_get_type(const ntg_object* object);
uint ntg_object_get_id(const ntg_object* object);

ntg_object* ntg_object_get_group_root(ntg_object* object);

/* Gets object's parent node. */
ntg_object* ntg_object_get_parent(ntg_object* object,
        ntg_object_get_parent_mode mode);

/* Returns base widget for node group(non-decorator). */
ntg_object* ntg_object_get_base_widget(ntg_object* object);

/* Returns children of object. These can include object decorators. */
const ntg_object_vec* ntg_object_get_children(const ntg_object* object);

bool ntg_object_is_ancestor(const ntg_object* object, const ntg_object* ancestor);
bool ntg_object_is_descendant(const ntg_object* object, const ntg_object* descendant);

/* ---------------------------------------------------------------- */

void ntg_object_set_min_size(ntg_object* object, struct ntg_xy size);
void ntg_object_set_max_size(ntg_object* object, struct ntg_xy size);

void ntg_object_unset_min_size(ntg_object* object);
void ntg_object_unset_max_size(ntg_object* object);

struct ntg_xy ntg_object_get_min_size(ntg_object* object);
struct ntg_xy ntg_object_get_natural_size(ntg_object* object);
struct ntg_xy ntg_object_get_max_size(ntg_object* object);

void ntg_object_set_grow(ntg_object* object, struct ntg_xy grow);
struct ntg_xy ntg_object_get_grow(const ntg_object* object);

/* ---------------------------------------------------------------- */

void ntg_object_layout(ntg_object* root, struct ntg_xy size);

/* ---------------------------------------------------------------- */

struct ntg_xy ntg_object_get_size(const ntg_object* object);
struct ntg_xy ntg_object_get_position_abs(const ntg_object* object);
struct ntg_xy ntg_object_get_position_rel(const ntg_object* object);

ntg_scene* ntg_object_get_scene(ntg_object* object);

const ntg_object_drawing* ntg_object_get_drawing(const ntg_object* object);
// ntg_object_drawing* ntg_object_get_drawing_(ntg_object* object);

/* ---------------------------------------------------------------- */

void ntg_object_listen(ntg_object* object, struct ntg_event_sub subscription);
void ntg_object_stop_listening(ntg_object* object, void* subscriber);

/* ---------------------------------------------------------------- */

bool ntg_object_feed_key(ntg_object* object, struct nt_key_event key_event,
        bool intercept);

// const ntg_object* ntg_object_get_focused(const ntg_object* object);
void ntg_object_focus(ntg_object* object, ntg_object* child, bool intercept);
ntg_object* ntg_object_get_focused(ntg_object* object);

/* ---------------------------------------------------------------- */

void ntg_object_tree_perform(ntg_object* object,
        ntg_object_perform_mode mode,
        void (*perform_fn)(ntg_object* object, void* data),
        void* data);

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED */
/* -------------------------------------------------------------------------- */

/* If `intercept` is true, the object is not directly focused,
 * but its child is. */

typedef bool (*ntg_object_process_key_fn)(ntg_object* object,
        struct nt_key_event key_event, bool intercept);

/* If `intercept` is true, the object is not directly focused,
 * but its child is. */
typedef void (*ntg_object_on_focus_fn)(ntg_object* object, bool intercept);

/* ---------------------------------------------------------------- */

struct ntg_object
{
    uint __id;
    ntg_object_type __type;

    struct
    {
        ntg_object* __parent;
        ntg_object_vec* __children;
    };

    struct
    {
        ntg_measure_fn __measure_fn;
        ntg_constrain_fn __constrain_fn;
        ntg_arrange_children_fn __arrange_children_fn;

        ntg_arrange_drawing_fn __arrange_drawing_fn;
    };

    struct
    {
        ntg_object_drawing* __drawing;
        ntg_cell __background;
    };

    struct
    {
        ntg_orientation __layout_orientation;

        struct ntg_xy __grow;

        struct ntg_xy __min_size, __natural_size,
                      __max_size, __size;
        
        bool __set_min_size, __set_max_size;

        struct ntg_xy __position;
    };

    struct 
    {
        ntg_object_process_key_fn __process_key_fn;
        ntg_object_on_focus_fn __on_focus_fn;
        ntg_object* __focused;
        // TODO
    };

    ntg_scene* __scene;

    ntg_listenable __listenable;
};

/* -------------------------------------------------------------------------- */
/* INTERNAL/PROTECTED API */
/* -------------------------------------------------------------------------- */

void __ntg_object_init__(ntg_object* object,
        ntg_object_type type,
        ntg_measure_fn measure_fn,
        ntg_constrain_fn constrain_fn,
        ntg_arrange_children_fn arrange_children_fn,
        ntg_arrange_drawing_fn arrange_drawing_fn);

void __ntg_object_deinit__(ntg_object* object);

/* ---------------------------------------------------------------- */

struct ntg_measure_result _ntg_object_measure(ntg_object* object,
        ntg_orientation orientation, size_t for_size,
        const ntg_measure_context* context);

void _ntg_object_constrain(ntg_object* object,
        ntg_orientation orientation, size_t size,
        const ntg_constrain_context* context,
        ntg_constrain_output* output);

void _ntg_object_arrange_children(ntg_object* object, struct ntg_xy size,
        const ntg_arrange_context* context,
        ntg_arrange_output* output);

void _ntg_object_arrange_drawing(ntg_object* object, struct ntg_xy size,
        ntg_object_drawing* drawing);

/* ---------------------------------------------------------------- */

void _ntg_object_set_background(ntg_object* object, ntg_cell background);

/* ---------------------------------------------------------------- */

void _ntg_object_root_set_scene(ntg_object* root, ntg_scene* scene);

/* ---------------------------------------------------------------- */

ntg_object_vec* _ntg_object_get_children(ntg_object* object);
void _ntg_object_add_child(ntg_object* object, ntg_object* child);
void _ntg_object_rm_child(ntg_object* object, ntg_object* child);

/* ---------------------------------------------------------------- */

void _ntg_object_set_process_key_fn(ntg_object* object,
        ntg_object_process_key_fn process_key_fn);
void _ntg_object_on_focus_key_fn(ntg_object* object,
        ntg_object_on_focus_fn on_focus_fn);

/* ---------------------------------------------------------------- */

ntg_object_drawing* _ntg_object_get_drawing(ntg_object* object);

#endif // _NTG_OBJECT_H_
