#ifndef _NTG_OBJECT_H_
#define _NTG_OBJECT_H_

#include "object/ntg_object_fwd.h"

#include "base/fwd/ntg_cell_fwd.h"
#include "object/util/ntg_object_vec.h"
#include "shared/ntg_xy.h"
#include <stddef.h>

#define NTG_PREF_SIZE_UNSET 0

/* -------------------------------------------------------------------------- */

ntg_object_t* ntg_object_get_parent(const ntg_object_t* object);

const ntg_object_vec_t* ntg_object_get_children(const ntg_object_t* object);

/* Sets root's natural size, constraints, position and. */
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

struct ntg_xy ntg_object_get_position_abs(const ntg_object_t* object);
struct ntg_xy ntg_object_get_position_rel(const ntg_object_t* object);

struct ntg_xy ntg_object_get_nsize(const ntg_object_t* object);

struct ntg_constr ntg_object_get_constr(const ntg_object_t* object);

struct ntg_xy ntg_object_get_size(const ntg_object_t* object);
struct ntg_xy ntg_object_get_content_size(const ntg_object_t* object);

struct ntg_xy ntg_object_get_pos(const ntg_object_t* object);
struct ntg_xy ntg_object_get_content_pos(const ntg_object_t* object);

const ntg_object_drawing_t* ntg_object_get_drawing(const ntg_object_t* object);

/* -------------------------------------------------------------------------- */

const ntg_cell_t* ntg_object_drawing_at(const ntg_object_drawing_t* drawing,
        struct ntg_xy pos);

struct ntg_xy ntg_object_drawing_get_size(const ntg_object_drawing_t* drawing);

#endif // _NTG_OBJECT_H_
