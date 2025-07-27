#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include <stddef.h>

#include "base/ntg_event.h"
#include "shared/ntg_xy.h"
#include "nt_event.h"
#include "object/ntg_border.h"

/* -------------------------------------------------------------------------- */
/* PUBLIC NTG_OBJECT API */
/* -------------------------------------------------------------------------- */

#define NTG_OBJECT(obj_ptr) ((ntg_object*)(obj_ptr))

#define NTG_PREF_SIZE_UNSET SIZE_MAX
#define NTG_PREF_SIZE_MAX (SIZE_MAX - 1)

typedef struct ntg_object ntg_object;
typedef struct ntg_object_vec ntg_object_vec;
typedef struct ntg_object_drawing ntg_object_drawing;
typedef struct ntg_scene ntg_scene;

/* -------------------------------------------------------------------------- */

/* OBJECT CONTENT - OBJECT EXCLUDING THE BORDERS */

/* Calculate and set the object's content natural size.
 * An object's content natural size represents the size it gravitates towards
 * naturally. This calculation must take into account preferred size.
 *
 * Use _ntg_object_set_natural_content_size() to set the size.
 *
 * Children's natural sizes are assumed to be set. */
typedef void (*ntg_natural_size_fn)(ntg_object* object);

/* ------------------------------------------------------ */

/* Calculate and set constraints of object's children. This calculation
 * should be performed based on their natural sizes.
 *
 * Use _ntg_object_set_constr() to set the constraints.
 *
 * Object's constraints are assumed to be set. Use the object's content
 * constraints for this calculation .*/
typedef void (*ntg_constrain_fn)(ntg_object* object);

/* ------------------------------------------------------ */

/* Calculate the final size of an object's content.
 *
 * Use _ntg_object_set_content_size() to set the size.
 *
 * Object's constraints and its children's sizes are assumed to be set. */
typedef void (*ntg_measure_fn)(ntg_object* object);

/* ------------------------------------------------------ */

/* Arrange object's content(children, drawing).
 *
 * To set the position of a child, use _ntg_object_set_pos_inside_content().
 * When setting the position, ignore the object's borders, they will be
 * accounted for automatically.
 *
 * The object's final size is assumed to be set. */
typedef void (*ntg_arrange_fn)(ntg_object* object);

/* ------------------------------------------------------ */

/* Returns whether the key has been processed(true) or ignored(false). */
typedef bool (*ntg_object_process_key_fn)(ntg_object* object,
        struct nt_key_event key_event);

/* Calculates border size depending on object constraints, natural border size and
 * natural content size.
 * Object's content will take up the remaining space depending on remaining
 * constraints. */
typedef struct ntg_border_size (*ntg_border_size_fn)(const ntg_object* object);

/* Only show borders if the object can fit all of its content on the screen */
struct ntg_border_size ntg_border_size_fn_show_if_can_fit(const ntg_object* object);

/* Show borders even if the content is not drawn. */
struct ntg_border_size ntg_border_size_fn_always_show(const ntg_object* object);

/* -------------------------------------------------------------------------- */

struct ntg_xy ntg_object_get_pref_size(const ntg_object* object);
struct ntg_xy ntg_object_get_pref_content_size(const ntg_object* object);

void ntg_object_set_pref_size(ntg_object* object, struct ntg_xy pref_size);

/* ------------------------------------------------------ */

struct ntg_xy ntg_object_get_natural_size(const ntg_object* object);
struct ntg_xy ntg_object_get_natural_content_size(const ntg_object* object);
struct ntg_border_size ntg_object_get_natural_border_size(const ntg_object* object);

/* ------------------------------------------------------ */

struct ntg_constr ntg_object_get_constr(const ntg_object* object);
struct ntg_constr ntg_object_get_content_constr(const ntg_object* object);

/* ------------------------------------------------------ */

struct ntg_xy ntg_object_get_size(const ntg_object* object);
struct ntg_xy ntg_object_get_content_size(const ntg_object* object);
struct ntg_border_size ntg_object_get_border_size(const ntg_object* object);

/* ------------------------------------------------------ */

struct ntg_xy ntg_object_get_pos_abs(const ntg_object* object);
struct ntg_xy ntg_object_get_pos_rel(const ntg_object* object);
struct ntg_xy ntg_object_get_content_pos_abs(const ntg_object* object);
struct ntg_xy ntg_object_get_content_pos_rel(const ntg_object* object);

/* ------------------------------------------------------ */

/* Sets root's constraints and position. */
void ntg_object_layout_root(ntg_object* root, struct ntg_xy root_size);

/* ------------------------------------------------------ */

/* Calculates and sets the natural size of `object`. 
 * If `object` is root, function returns immediately. */
void ntg_object_calculate_natural_size(ntg_object* object);

/* ------------------------------------------------------ */

/* Calculates and sets the constraints of `object`'s children. */
void ntg_object_constrain(ntg_object* object);

/* ------------------------------------------------------ */

/* Calculates and sets the final size of an object.
 * If `object` is root, function returns immediately. */
void ntg_object_measure(ntg_object* object);

/* ------------------------------------------------------ */

/* Arranges the content inside `object`. */
void ntg_object_arrange(ntg_object* object);

/* ------------------------------------------------------ */

void ntg_object_set_border_style(ntg_object* object,
        struct ntg_border_style border_style);
void ntg_object_set_border_pref_size(ntg_object* object,
        struct ntg_border_size border_size);
void ntg_object_set_border_size_fn(ntg_object* object,
        ntg_border_size_fn border_size_fn);

/* ------------------------------------------------------ */

bool ntg_object_feed_key(ntg_object* object, struct nt_key_event key_event);

/* ------------------------------------------------------ */

void ntg_object_listen(ntg_object* object, struct ntg_event_sub subscription);

void ntg_object_stop_listening(ntg_object* object, void* subscriber);

/* ------------------------------------------------------ */

/* Called internally by ntg_scene when root changes. */
void _ntg_object_set_scene(ntg_object* root, ntg_scene* scene);

/* -------------------------------------------------------------------------- */
/* INTERNAL NTG_OBJECT API */
/* -------------------------------------------------------------------------- */

struct ntg_object
{
    ntg_object* _parent;
    ntg_object_vec* _children;

    struct ntg_xy __pref_size;

    struct
    {
        struct ntg_border_size _border_pref_size;
        struct ntg_border_style _border_style;
        ntg_border_size_fn __border_size_fn;
    };

    struct
    {
        ntg_object_drawing* _virtual_drawing;
        bool __scroll;
        struct ntg_xy __curr_scroll;
        struct ntg_dxy __buffered_scroll; // buffered until object is remeasured

        ntg_object_drawing* _full_drawing;
    };

    struct
    {
        struct ntg_xy __natural_size;
        /* Must exist because of potential sync issues - pref border size can be
         * set in between natural size calculations. This means that
         * ntg_object_get_natural_content_size() will return an invalid value
         * (the value was previously calculated by subtracting border pref size
         * from natural size). */
        struct ntg_border_size __natural_border_size;

        struct ntg_constr __constraints;
        struct ntg_border_size __border_constr;

        struct ntg_xy __size;
        struct ntg_border_size __border_size;

        struct ntg_xy __pos; // relative
    };

    struct
    {
        ntg_natural_size_fn __natural_size_fn;
        ntg_constrain_fn __constrain_fn;
        ntg_measure_fn __measure_fn;
        ntg_arrange_fn __arrange_fn;
    };

    ntg_object_process_key_fn __process_key_fn;

    ntg_scene* _scene;

    ntg_listenable __listenable;
};

/* ntg_object protected */

void __ntg_object_init__(ntg_object* object,
        ntg_natural_size_fn natural_size_fn,
        ntg_constrain_fn constrain_fn,
        ntg_measure_fn measure_fn,
        ntg_arrange_fn arrange_fn);

void __ntg_object_deinit__(ntg_object* object);

/* ------------------------------------------------------ */

/* Configure object behavior */
void _ntg_object_set_process_key_fn(ntg_object* object,
        ntg_object_process_key_fn process_key_fn);

/* ------------------------------------------------------ */

void _ntg_object_set_natural_content_size(ntg_object* object, struct ntg_xy size);
void _ntg_object_set_constr(ntg_object* object, struct ntg_constr constr);
void _ntg_object_set_content_size(ntg_object* object, struct ntg_xy content_size);
void _ntg_object_set_pos_inside_content(ntg_object* object, struct ntg_xy pos);

/* ------------------------------------------------------ */

void _ntg_object_child_add(ntg_object* parent, ntg_object* child);
void _ntg_object_child_remove(ntg_object* parent, ntg_object* child);

/* ------------------------------------------------------ */

void _ntg_object_scroll_enable(ntg_object* object);
void _ntg_object_scroll_disable(ntg_object* object);
void _ntg_object_scroll(ntg_object* object, struct ntg_dxy offset_diff);
struct ntg_xy _ntg_object_get_scroll(const ntg_object* object);

/* ------------------------------------------------------ */

void _ntg_object_perform_tree(ntg_object* root,
        void (*perform_fn)(ntg_object* curr_obj, void* data),
        void* data);

/* -------------------------------------------------------------------------- */
/* EVENT TYPE DATA STRUCTS */
/* -------------------------------------------------------------------------- */

struct ntg_pref_size_change
{
    struct ntg_xy old, new;
};

#endif // _NTG_OBJECT_H_
