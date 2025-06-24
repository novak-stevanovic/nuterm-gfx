#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include "shared/ntg_xy.h"
#include "nt_event.h"
#include <stddef.h>

#define NTG_PREF_SIZE_UNSET 0

/* -------------------------------------------------------------------------- */

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
 * Object's constraints are assumed to be set. */
typedef void (*ntg_constrain_fn)(ntg_object* object);

/* Calculate the final size of an object.
 *
 * Object's constraints and its children's sizes are assumed to be set. */
typedef void (*ntg_measure_fn)(ntg_object* object);

/* Arrange object's inner content(children, drawing).
 *
 * The object's final size is assumed to be set. */
typedef void (*ntg_arrange_fn)(ntg_object* object);

typedef bool (*ntg_process_key_fn)(ntg_object* object,
        struct nt_key_event key_event);

/* -------------------------------------------------------------------------- */

ntg_object* ntg_object_get_parent(ntg_object* object);

const ntg_object_vec* ntg_object_get_children(const ntg_object* object);

/* Sets root's constraints, position and. */
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

struct ntg_xy ntg_object_get_pref_size(const ntg_object* object);
void ntg_object_set_pref_size(ntg_object* object, struct ntg_xy pref_size);

struct ntg_xy ntg_object_get_position_abs(const ntg_object* object);
struct ntg_xy ntg_object_get_position_rel(const ntg_object* object);

struct ntg_xy ntg_object_get_nsize(const ntg_object* object);

struct ntg_constr ntg_object_get_constr(const ntg_object* object);

struct ntg_xy ntg_object_get_size(const ntg_object* object);
struct ntg_xy ntg_object_get_content_size(const ntg_object* object);

struct ntg_xy ntg_object_get_pos(const ntg_object* object);
struct ntg_xy ntg_object_get_content_pos(const ntg_object* object);

const ntg_object_drawing* ntg_object_get_drawing(const ntg_object* object);

bool ntg_object_feed_key_event(ntg_object* object, struct nt_key_event key_event);

bool ntg_object_is_focusable(const ntg_object* object);

ntg_scene* ntg_object_get_scene(const ntg_object* object);

/* Called internally by ntg_scene */
void _ntg_object_set_scene(ntg_object* root, ntg_scene* scene);

/* -------------------------------------------------------------------------- */

#endif // _NTG_OBJECT_H_
