#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include <stddef.h>

#include "shared/ntg_xy.h"
#include "nt_event.h"
#include "object/ntg_object_border.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC */
/* -------------------------------------------------------------------------- */

#define NTG_PREF_SIZE_UNSET 0

typedef struct ntg_object ntg_object;
typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_scene ntg_scene;

/* Calculate and set the object's natural size. An object's natural size
 * represents the size it gravitates towards naturally. This calculation
 * must take into account preferred size.
 *
 * Children's natural sizes are assumed to be set. */
typedef void (*ntg_nsize_fn)(ntg_object* object);

/* Calculate and set constraints of object's children. This calculation
 * should be performed based on their natural sizes.
 *
 * Object's constraints are assumed to be set. Use the object's content
 * constraints for this calculation .*/
typedef void (*ntg_constrain_fn)(ntg_object* object);

/* Calculate the final size of an object.
 *
 * Object's constraints and its children's sizes are assumed to be set. */
typedef void (*ntg_measure_fn)(ntg_object* object);

/* Arrange object's inner content(children, drawing).
 *
 * The object's final size is assumed to be set. */
typedef void (*ntg_arrange_fn)(ntg_object* object);

/* Returns whether the key has been processed(true) or ignored(false). */
typedef bool (*ntg_object_process_key_fn)(ntg_object* object,
        struct nt_key_event key_event);

/* This is done at the beginning of `constrain phase`. The object should
 * calculate border sizes depending on the min. constraints. The function
 * is called on _ntg_object_set_constr(). */
typedef void (*ntg_calculate_border_size_fn)(ntg_object* object);

/* -------------------------------------------------------------------------- */

#define NTG_OBJECT(obj_ptr) ((ntg_object*)(obj_ptr))

/* Sets root's constraints and position. */
void ntg_object_layout_root(ntg_object* root, struct ntg_xy root_size);

/* Calculates and sets the natural size of `object`. 
 * If `object` is root, function returns immediately. */
void ntg_object_calculate_nsize(ntg_object* object);

/* Calculates and sets the constraints of `object`'s children. */
void ntg_object_constrain(ntg_object* object);

/* Calculates and sets the final size of an object.
 * If `object` is root, function returns immediately. */
void ntg_object_measure(ntg_object* object);

/* Arranges the content inside `object`. */
void ntg_object_arrange(ntg_object* object);

void ntg_object_set_pref_size(ntg_object* object, struct ntg_xy pref_size);

struct ntg_xy ntg_object_get_pos_rel(const ntg_object* object);
struct ntg_xy ntg_object_get_content_pos(const ntg_object* object);

struct ntg_xy ntg_object_get_content_constr(const ntg_object* object);

/* Calculates content size based on current size and border width. */
struct ntg_xy ntg_object_get_content_size(const ntg_object* object);

bool ntg_object_feed_key(ntg_object* object, struct nt_key_event key_event);

/* Called internally by ntg_scene when root changes. */
void _ntg_object_set_scene(ntg_object* root, ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* INTERNAL */
/* -------------------------------------------------------------------------- */

struct ntg_object
{
    ntg_object* _parent;
    ntg_object_vec* _children;

    struct ntg_object_border _border_north, _border_east,
            _border_south, _border_west;

    bool __scroll;
    struct ntg_dxy __buffered_scroll; // buffered until object is remeasured
    ntg_object_drawing* _drawing;

    struct ntg_xy _pref_size;
    struct ntg_xy _nsize;
    struct ntg_constr _constr;
    struct ntg_xy _size;
    struct ntg_xy _pos; // absolute

    ntg_nsize_fn __nsize_fn;
    ntg_constrain_fn __constrain_fn;
    ntg_measure_fn __measure_fn;
    ntg_arrange_fn __arrange_fn;

    ntg_object_process_key_fn __process_key_fn;
    ntg_calculate_border_size_fn __calculate_border_size_fn;

    /* Cached */
    size_t __border_north_width, __border_south_width,
           __border_east_width, __border_west_width;

    ntg_scene* _scene;
};

/* ntg_object protected */

void __ntg_object_init__(ntg_object* object, ntg_nsize_fn nsize_fn,
        ntg_constrain_fn constrain_fn, ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn);

void __ntg_object_deinit__(ntg_object* object);

void _ntg_object_set_nsize(ntg_object* object, struct ntg_xy size);
void _ntg_object_set_constr(ntg_object* object, struct ntg_constr constr);
void _ntg_object_set_size(ntg_object* object, struct ntg_xy size);
void _ntg_object_set_pos(ntg_object* object, struct ntg_xy pos);

void _ntg_object_child_add(ntg_object* parent, ntg_object* child);
void _ntg_object_child_remove(ntg_object* parent, ntg_object* child);

void _ntg_object_scroll_enable(ntg_object* object);
void _ntg_object_scroll_disable(ntg_object* object);
void _ntg_object_scroll(ntg_object* object, struct ntg_dxy offset_diff);
struct ntg_xy _ntg_object_get_scroll(const ntg_object* object);

void _ntg_object_set_process_key_fn(ntg_object* object,
        ntg_object_process_key_fn process_key_fn);
void _ntg_object_set_calculate_border_size_fn(ntg_object*
        ntg_calculate_border_size_fn calculate_border_size_fn);

void _ntg_object_perform_tree(ntg_object* root,
        void (*perform_fn)(ntg_object* curr_obj, void* data),
        void* data);

#endif // _NTG_OBJECT_H_
