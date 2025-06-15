#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include "shared/ntg_xy.h"
#include <stddef.h>

#define NTG_PREF_SIZE_UNSET 0

/* -------------------------------------------------------------------------- */

typedef struct ntg_object ntg_object_t;
typedef struct ntg_object_vec ntg_object_vec_t;
typedef struct ntg_object_drawing ntg_object_drawing_t;

/* Calculate and set the object's natural size. An object's natural size
 * represents the size it gravitates towards naturally. This calculation
 * must take into account preferred size.
 *
 * Children's natural sizes are assumed to be set. */
typedef void (*ntg_nsize_fn)(ntg_object_t* object);

/* Calculate and set constraints of object's children. This calculation
 * should be performed based on their natural sizes.
 *
 * Object's constraints are assumed to be set. */
typedef void (*ntg_constrain_fn)(ntg_object_t* object);

/* Calculate the final size of an object.
 *
 * Object's constraints and its children's sizes are assumed to be set. */
typedef void (*ntg_measure_fn)(ntg_object_t* object);

/* Arrange object's inner content(children, drawing).
 *
 * The object's final size is assumed to be set. */
typedef void (*ntg_arrange_fn)(ntg_object_t* object);

/* -------------------------------------------------------------------------- */

ntg_object_t* ntg_object_get_parent(const ntg_object_t* object);

const ntg_object_vec_t* ntg_object_get_children(const ntg_object_t* object);

/* Sets root's constraints, position and. */
void ntg_object_layout_root(ntg_object_t* root, struct ntg_xy root_size);

/* Calculates and sets the natural size of `object`. 
 * If `object` is root, function returns immediately. */
void ntg_object_calculate_nsize(ntg_object_t* object);

/* Calculates and sets the constraints of `object`'s children. */
void ntg_object_constrain(ntg_object_t* object);

/* Calculates and sets the final size of an object.
 * If `object` is root, function returns immediately. */
void ntg_object_measure(ntg_object_t* object);

/* Arranges the content inside `object`. */
void ntg_object_arrange(ntg_object_t* object);

struct ntg_xy ntg_object_get_pref_size(const ntg_object_t* object);
void ntg_object_set_pref_size(ntg_object_t* object, struct ntg_xy pref_size);

struct ntg_xy ntg_object_get_position_abs(const ntg_object_t* object);
struct ntg_xy ntg_object_get_position_rel(const ntg_object_t* object);

struct ntg_xy ntg_object_get_nsize(const ntg_object_t* object);

struct ntg_constr ntg_object_get_constr(const ntg_object_t* object);

struct ntg_xy ntg_object_get_size(const ntg_object_t* object);
struct ntg_xy ntg_object_get_content_size(const ntg_object_t* object);

struct ntg_xy ntg_object_get_pos(const ntg_object_t* object);
struct ntg_xy ntg_object_get_content_pos(const ntg_object_t* object);

const ntg_object_drawing_t* ntg_object_get_drawing(const ntg_object_t* object);

struct ntg_xy ntg_object_get_vsize(const ntg_object_t* object);

/* -------------------------------------------------------------------------- */

#endif // _NTG_OBJECT_H_
