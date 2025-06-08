#ifndef _NTG_OBJECT_FWD_H_
#define _NTG_OBJECT_FWD_H_

#include "shared/ntg_xy.h"

typedef struct ntg_object ntg_object_t;
typedef struct ntg_pane ntg_pane_t;
typedef struct ntg_container ntg_container_t;
typedef struct ntg_color_block ntg_color_block_t;
typedef struct ntg_box ntg_box_t;

typedef struct ntg_object_drawing ntg_object_drawing_t;

/* Calculate and set the object's natural size. An object's natural size
 * represents the size it gravitates towards naturally. This calculation
 * must take into account preferred size. */
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

#endif // _NTG_OBJECT_FWD_H_

